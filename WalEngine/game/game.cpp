#include "game.h"

void Game::init()
{
	albedo = Texture("pbr/plastic/albedo.png");
	normal = Texture("pbr/plastic/normal.png");
	metallic = Texture("pbr/plastic/metallic.png");
	roughness = Texture("pbr/plastic/roughness.png");
	ao = Texture("pbr/plastic/ao.png");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	albedo.process();
	normal.process();
	metallic.process();
	roughness.process();
	ao.process();

	mat = new Material("pbr");
	mat->set_shader("pbr.vert", "ibl.frag");
	mat_layered = new  Material("layered_material");
	mat_layered->set_shader("pbr.vert", "layered_material.frag");

	mat->set_texture("albedoMap", albedo);
	mat->set_texture("normalMap", normal);
	mat->set_texture("metallicMap", metallic);
	mat->set_texture("roughnessMap", roughness);

	mat_layered->set_texture("albedoMap", albedo);
	mat_layered->set_texture("normalMap", normal);
	mat_layered->set_texture("metallicMap", metallic);
	mat_layered->set_texture("roughnessMap", roughness);

	RenderEngine::set_sampler_slot("irradianceMap", 0);
	RenderEngine::set_sampler_slot("prefilterMap", 1);
	RenderEngine::set_sampler_slot("brdfLUT", 2);
	RenderEngine::set_sampler_slot("albedoMap", 3);
	RenderEngine::set_sampler_slot("normalMap", 4);
	RenderEngine::set_sampler_slot("metallicMap", 5);
	RenderEngine::set_sampler_slot("roughnessMap", 6);

	model.mTransform->set_pos(glm::vec3(-1.5, -1.0, 0.0));
	model2.mTransform->set_pos(glm::vec3(1.5, -1.0, 0.0));

	model.set_mat(mat);
	model2.set_mat(mat_layered);

	renderRoot.add_child(&model);
	renderRoot.add_child(&model2);

	Entity::Root.push_back(&renderRoot);

	Material::update_uniforms_constant_all();
}

void Game::render(RenderEngine &renderer)
{
	glm::vec2 curScreen;

	curScreen.x = 5 * float((Window::Inputs.get_mouse_x()) - Window::Inputs.get_win_size_x() / 2.0f) / float(Window::Inputs.get_win_size_x());
	curScreen.y = 5 * float(0 - (Window::Inputs.get_mouse_y()) - Window::Inputs.get_win_size_y() / 2.0f) / float(Window::Inputs.get_win_size_y());
	glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 lightColor = glm::vec3(255.0f, 255.0f, 255.0f);

	dir.get_component<PointLightCom>()->set_color(lightColor);
	dir.mTransform->set_pos(glm::vec3(-1.5, -1.0, 4.0));
	dir2.get_component<PointLightCom>()->set_color(lightColor);
	dir2.mTransform->set_pos(glm::vec3(1.5, -1.0, 4.0));

	mat->mShader->use();
	mat->mShader->set_int("irradianceMap", 0);
	mat->mShader->set_int("prefilterMap", 1);
	mat->mShader->set_int("brdfLUT", 2);

	mat_layered->mShader->use();
	mat_layered->mShader->set_int("irradianceMap", 0);
	mat_layered->mShader->set_int("prefilterMap", 1);
	mat_layered->mShader->set_int("brdfLUT", 2);

	//renderer.add_light(dir);
	RenderEngine::Lights.push_back(&dir);
	RenderEngine::Lights.push_back(&dir2);
	renderer.render(renderRoot);
	//cube1.render(shader2);
	//model.draw(shader);
}