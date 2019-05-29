#include "game.h"

void Game::init()
{
	albedo = Texture("pbr/rusted_iron/albedo.png");
	normal = Texture("pbr/rusted_iron/normal.png");
	metallic = Texture("pbr/rusted_iron/metallic.png");
	roughness = Texture("pbr/rusted_iron/roughness.png");
	ao = Texture("pbr/rusted_iron/ao.png");

	Sphere::load();
	Cube::load();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	albedo.process();
	normal.process();
	metallic.process();
	roughness.process();
	ao.process();

	mat = new Material("pbr");
	mat->set_shader("pbr.vert", "pbr.frag");

	mat->set_texture("albedoMap", albedo);
	mat->set_texture("normalMap", normal);
	mat->set_texture("metallicMap", metallic);
	mat->set_texture("roughnessMap", roughness);

	RenderEngine::set_sampler_slot("albedoMap", 0);
	RenderEngine::set_sampler_slot("normalMap", 1);
	RenderEngine::set_sampler_slot("metallicMap", 2);
	RenderEngine::set_sampler_slot("roughnessMap", 3);

	sphere1.mTransform->set_pos(glm::vec3(1.0, 0.0, 0.0));
	sphere1.add_child(&sphere2);
	sphere2.add_child(&cube1);
	sphere2.mTransform->set_pos(glm::vec3(1.0, 1.0, 0.0));
	cube1.mTransform->set_pos(glm::vec3(-1.0, 0.0, 0.0));
	cube1.mTransform->set_rot(Quaternion(glm::vec3(1.0, 1.0, 0.0), glm::pi<float>() * 1 / 6));

	sphere1.set_mat(mat);
	sphere2.set_mat(mat);
	//cube1.set_shader(shader);

	root.add_child(&sphere1);
	root.add_child(&sphere4);

	Material::update_uniforms_constant_all();
}

void Game::render(RenderEngine &renderer)
{
	glm::vec2 curScreen;

	curScreen.x = 5 * float((Window::Inputs.get_mouse_x()) - Window::Inputs.get_win_size_x() / 2.0f) / float(Window::Inputs.get_win_size_x());
	curScreen.y = 5 * float(0 - (Window::Inputs.get_mouse_y()) - Window::Inputs.get_win_size_y() / 2.0f) / float(Window::Inputs.get_win_size_y());
	glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec3 lightColor = glm::vec3(255.0f, 255.0f, 255.0f);

	dir.get_component<PointLightCom>()->set_color(lightColor);
	dir.mTransform->set_pos(lightPosition + glm::vec3(curScreen, 0.0));

	//renderer.add_light(dir);
	renderer.add_light(dir); 
	renderer.render(root);
	//cube1.render(shader2);
	//model.draw(shader);
}