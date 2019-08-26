#include "forwardplus.h"
void draw_quad();

void ForwardPlus::gui()
{
// 	bool flag = true;
// 	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
// 	if (!ImGui::Begin("LayerMaterialPanel", &flag))
// 	{
// 		ImGui::End();
// 		return;
// 	}
// 
// 	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
// 
// 	ImGui::PopStyleVar();
// 	ImGui::End();
}



void ForwardPlus::precompute()
{
	FrameBuffer capture(64, 64);

	FrameBuffer::bind_render_targer_reset();

	FGDTexture = Texture(64, 64, 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true);

	capture.change_render_buffer_storage(64, 64);
	capture.bind_texture(FGDTexture.get_ID()[0]);

	glViewport(0, 0, 64, 64);
	fgdShader.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw_quad();

	FrameBuffer::bind_render_targer_reset();
}

void ForwardPlus::init()
{
	MainTex = Texture("pbr/multilayer/CarbonFiber_BC.png");
	normalMap = Texture("pbr/multilayer/CarbonFiber_NM.png");
	WoodTex = Texture("toy_box_diffuse.png");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	MainTex.process();
	normalMap.process();
	WoodTex.process();

	mat = new Material("pbr");
	mat->set_shader("pbr.vert", "forwardplus.frag");

	mat->set_texture("MainTex", MainTex);
	mat->set_texture("RoughnessMap", RoughnessMap);
	mat->set_texture("CoatNormalMap", coatNormalMap);
	mat->set_texture("OcclusionMap", OcclusionMap);
	mat->set_texture("BentNormal", BentNormal);
	mat->set_texture("GeomNormal", GeomNormal);
	mat->set_texture("BumpMap", normalMap);

	//mat2 = new Material("pbr2");
	//mat2->set_shader("pbr.vert", "forwardplus.frag");

	//mat2->set_texture("MainTex", WoodTex);
	//mat2->set_texture("RoughnessMap", RoughnessMap);
	//mat2->set_texture("CoatNormalMap", coatNormalMap);
	//mat2->set_texture("OcclusionMap", OcclusionMap);
	//mat2->set_texture("BentNormal", BentNormal);
	//mat2->set_texture("GeomNormal", GeomNormal);
	//mat2->set_texture("BumpMap", normalMap);

	RenderEngine::set_sampler_slot("RoughnessMap", 7);
	RenderEngine::set_sampler_slot("CoatNormalMap", 8);
	RenderEngine::set_sampler_slot("OcclusionMap", 9);
	RenderEngine::set_sampler_slot("BumpMap", 10);
	RenderEngine::set_sampler_slot("MainTex", 11);

	//init scene
	glm::vec3 pos = glm::vec3(0.0);
	int num = ceil(sqrt(OBJECT_NUM));

	for (int i = 0; i < OBJECT_NUM; i++)
	{
		int col = i / num;
		int raw = i % num;
		models[i].mTransform->set_pos(pos + glm::vec3(col * INTERAL, 0.0 , raw * INTERAL));
		models[i].set_mat(mat);
		models[i].mRenderStateEvent = [&]() {  FGDTexture.bind(2); };
		renderRoot.add_child(&models[i]);
	}

	Entity::Root.push_back(&renderRoot);

	//init compute shader
	workgroupsx = (Window::Inputs.get_win_size_x() + (Window::Inputs.get_win_size_x() % 16)) / 16;
	workgroupsy = (Window::Inputs.get_win_size_y() + (Window::Inputs.get_win_size_y() % 16)) / 16;

	size_t numtiles = workgroupsx * workgroupsy;
	size_t headsize = 16;
	size_t nodesize = 16;

	glGenBuffers(1, &headbuffer);
	glGenBuffers(1, &nodebuffer);
	glGenBuffers(1, &lightbuffer);
	glGenBuffers(1, &counterbuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, headbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numtiles * headsize, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, nodebuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numtiles * nodesize * 1024, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightbuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_LIGHTS * sizeof(PointLightData), 0, GL_DYNAMIC_DRAW);

	if (lightbuffer == 0)
		return;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightbuffer);
	PointLightData* particles = (PointLightData*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	int segments = sqrt(NUM_LIGHTS);
	float theta, phi;

	glm::vec3 randomcolors[3] =
	{
		glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 1)
	};

	for (int i = 0; i < segments; ++i)
	{
		for (int j = 0; j < segments; ++j)
		{
			PointLightData& p = particles[i * segments + j];

			p.intensity = 1.0;
			p.constant = 1.0;
			p.exponent = 1.0;
			p.linear = 1.0;
			p.position = glm::vec3(-1.5 + i * INTERAL, 0.0, -1.5 + j * INTERAL);
			p.range = LIGHT_RADIUS;
			p.color = randomcolors[(i + j) % 3];
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterbuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	lightcull.use();
	lightcull.set_int("depthSampler", 0);
	lightcull.set_int("numLights", NUM_LIGHTS);

	Material::update_uniforms_constant_all();
}

void ForwardPlus::render(RenderEngine &renderer)
{
	glm::vec2 curScreen;

	curScreen.x = 5 * float((Window::Inputs.get_mouse_x()) - Window::Inputs.get_win_size_x() / 2.0f) / float(Window::Inputs.get_win_size_x());
	curScreen.y = 5 * float(0 - (Window::Inputs.get_mouse_y()) - Window::Inputs.get_win_size_y() / 2.0f) / float(Window::Inputs.get_win_size_y());
	glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 lightColor = glm::vec3(255.0f, 255.0f, 255.0f);

	dir.get_component<DirectionalLightCom>()->set_color(lightColor);
	dir.get_component<DirectionalLightCom>()->set_intensity(0.3);

	mat->mShader->use();
	mat->mShader->set_int("prefilterMap", 1);
	mat->mShader->set_int("_PreFGDandDisneyDiffuse", 2);

	RenderEngine::Lights.push_back(&dir);
	renderer.render(renderRoot);
}