#include "stdafx.h"
#include "showMaster.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//									Inicializacion de las variables externas definidas en "includes.h"											//
char key_pressed = 0;			// guarda la ultima tecla presionada
int control = 0;				// objeto controlado por el usuario
float universe_scale = 1.0f;	// escala del universo
float dt_a = 0.0f;				// el tiempo delta principal
float dt_b = 0.0f;				// el tiempo delta del motor
int time_acceleration = 0;		// the time acceleration, surprise!
int total_objects = 0;
ID3D11Device* D3D_device = nullptr;
ID3D11DeviceContext* D3D_context = nullptr;
vec3 aether_velocity = { 0.0, 0.0, 0.0 };
//																																				//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

showMasterClass::showMasterClass()
{
	screenNear = 0.01f; // 0.4f;
	screenFar = 1.0e9f;
	time_acceleration = 1;
	starshine = 0.8f;
	default_ambient_ilumination = 0.15f; // 0.055f;
	total_initial_setup_objects = 4;
	star_object = total_initial_setup_objects;
	rot_angle_before = 0.0;
	conv_angle_before = 0.0;
	calculations_epoch = 0.0;
	true_julian = 0.0;
	clock_rotation_time = 0.0;
	full_date = 0.0;
	near_ = 0.0f;
	far_ = 0.0f;
	under_clouds = 0.0f;
	hit_face = 0;
	landing_pad_view_altitude = 0.1f;
	bullet_range = 1000.f; // rango de distancia de los objetos que entran en el calculo de colisiones de bullet, fuera de este rango son excluidos

	hit_single_face = { { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f, } };
	for (int n = 0; n < 13; n++) tf[n] = { { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f, } };

	SHADOWMAP_WIDTH = 2048;
	SHADOWMAP_HEIGHT = 2048;

	SHADOWMAP_DEPTH = 100.0f;
	SHADOWMAP_NEAR = 0.1f;

	light_view_position_to_cast_shadows = 3.0f;

	threadize = false;
	proceed = true;
	cut_engine = false;
	max_out = false;
	inversed_render_ = false;
	simplified_atmosphere = false;
	GUI_visible = true;
	ready_to_render = false;
	sync_ = false;
	switched_shadows = true;
	proceed = false;
	start_synchronized = false;
	bullet_reload = false;
	freeze_aether_physics = false;

	in_cockpit = false;
	switched_camera = false;

	camera_in_atmosphere = 0.0f;
	camera_altitude = 0.0f;

	minimum_gravity_influence_weight = 1.0e10;
	screenWidth = 1366;
	screenHeight = 768;
	material_check = -1;
	total_objects = 0;
	default_models = 11;
	models_number = default_models;
	default_textures = 2;
	textures_number = default_textures;
	control = total_initial_setup_objects; // default focus object if one is not provided by the objects file
	total_ventanas = 0;

	win_system = nullptr;
	DirectX = nullptr;
	input = nullptr;
	Basic_shader = nullptr;
	Atmosphere_shader = nullptr;
	camera = nullptr;
	model = nullptr;
	texture = nullptr;
	main_fps = nullptr;
	physics_engine_fps = nullptr;
	texture_array[0] = nullptr;
	texture_array[1] = nullptr;
	texture_array[2] = nullptr;
	GUI = nullptr;
	light_ = nullptr;
	color = nullptr;
	stars = nullptr;
	auto_ = nullptr;
	aero_dynamics = nullptr;
	background_texture = nullptr;
	m_TextureShader = nullptr;
	object_creator = nullptr;
	script_ = nullptr;
	animation = nullptr;
	shock_diamonds_particles = nullptr;
	bullet_physics = nullptr;

	instanced_shader = nullptr;

	m_RenderTexture = nullptr;
	m_RenderTextureFullScreen = nullptr;
	m_DepthShader = nullptr;
	m_DepthShader_tr = nullptr;
}

showMasterClass::showMasterClass(const showMasterClass& other){}

showMasterClass::~showMasterClass()
{
	SAFE_DELETE(bullet_physics);
	SAFE_DELETE(animation);
	SAFE_DELETE(script_);
	SAFE_DELETE(object_creator);
	SAFE_DELETE(m_RenderTexture);
	SAFE_DELETE(m_RenderTextureFullScreen);
	SAFE_DELETE(m_DepthShader_tr);
	SAFE_DELETE(m_DepthShader);
	SAFE_DELETE(shock_diamonds_particles);

	SAFE_DELETE(instanced_shader);

	SAFE_DELETE(m_TextureShader);
	SAFE_DELETE(background_texture);
	SAFE_DELETE(aero_dynamics);
	SAFE_DELETE(auto_);
	SAFE_DELETE(stars);
	SAFE_DELETE(color);
	SAFE_DELETE(light_);
	SAFE_DELETE(GUI);
	SAFE_RELEASE(texture_array[0]);
	SAFE_RELEASE(texture_array[1]);
	SAFE_RELEASE(texture_array[2]);
	SAFE_DELETE(main_fps);
	SAFE_DELETE(physics_engine_fps);
	SAFE_DELETE_ARRAY(texture);
	SAFE_DELETE_ARRAY(model);
	SAFE_DELETE(camera);
	SAFE_DELETE(input);
	SAFE_DELETE(Basic_shader);
	SAFE_DELETE(Atmosphere_shader);
	SAFE_DELETE(DirectX);
	SAFE_DELETE(win_system);
}

bool showMasterClass::Initialize(bool full_screen_resizeable, int screen_width, int screen_height, bool vsync, bool full_screen)
{
	win_system = new win_systemClass; // Inicializamos la ventana de Microsoft Windows
	if (!win_system->Initialize(full_screen_resizeable, full_screen, screen_width, screen_height))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the main window.", "Error", MB_OK);
		return false;
	}

	DirectX = new directx11;  // Inicializamos DirectX 11 Graphics
	if (!DirectX->Initialize(full_screen_resizeable, screen_width, screen_height, vsync, win_system->getHWND(), full_screen, screenNear, screenFar))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize DirectX 11.", "Error", MB_OK);
		return false;
	}

	D3D_device = DirectX->GetDevice();
	D3D_context = DirectX->GetDeviceContext();

	if (full_screen && !full_screen_resizeable)
	{
		screen_width = GetSystemMetrics(SM_CXSCREEN);
		screen_height = GetSystemMetrics(SM_CYSCREEN);
	}

	screenWidth = screen_width;
	screenHeight = screen_height;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//												Initialize the shaders														//

	Basic_shader = new Basic_shader_class;
	if (!Basic_shader->Initialize(win_system->getHWND()))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the Basic shader object.", "Error", MB_OK);
		return false;
	}

	Atmosphere_shader = new Atmosphere_shader_class;
	if (!Atmosphere_shader->Initialize(win_system->getHWND()))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the Atmosphere shader object.", "Error", MB_OK);
		return false;
	}

	// Create the instantiated model object.
	shock_diamonds_particles = new InstancedModelClass(object);
	if (!shock_diamonds_particles) return false;

	// Initialize the model object.
	if (!shock_diamonds_particles->Initialize("textures/star4.png", 0))
	{
		MessageBoxA(m_hwnd, "Could not initialize the instanced model object.", "Error", MB_OK);
		return false;
	}

	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) return false;

	// Initialize the render to texture object.
	if (!m_RenderTexture->Initialize(SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the render to texture object.", "Error", MB_OK);
		return false;
	}

	// Create the render to texture object.
	m_RenderTextureFullScreen = new RenderTextureClass;
	if (!m_RenderTextureFullScreen) return false;

	// Initialize the render to texture object.
	if (!m_RenderTextureFullScreen->Initialize(screenWidth, screenHeight, 0.1f, 1000.f))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the render to texture object.", "Error", MB_OK);
		return false;
	}

	// Create the depth shader object.
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader)
	{
		return false;
	}

	// Initialize the depth shader object.
	if (!m_DepthShader->Initialize(win_system->getHWND()))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the depth shader object.", "Error", MB_OK);
		return false;
	}

	// Create the transparent depth shader object.
	m_DepthShader_tr = new DepthShaderClass_tr;
	if (!m_DepthShader_tr)
	{
		return false;
	}

	// Initialize the depth shader object.
	if (!m_DepthShader_tr->Initialize(win_system->getHWND()))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the transparent depth shader object.", "Error", MB_OK);
		return false;
	}
	//																															//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	input = new directx_input; // Inicializamos DirectX Input
	if (!input->Initialize(win_system->getHINSTANCE(), win_system->getHWND(), screen_width, screen_height))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize DirectX Input.", "Error", MB_OK);
		return false;
	}

	bullet_physics = new BulletClass(object);
	if (!bullet_physics || !bullet_physics->Initialize())
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Can't create the Bullet physics", "Error", MB_OK);
		return false;
	}

	camera = new CameraClass(object);
	if (!camera)
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the camera.", "Error", MB_OK);
		return false;
	}

	object_creator = new ObjectCreatorClass(object, simplified_atmosphere, total_initial_setup_objects, starshine, SHADOWMAP_DEPTH,
		texture_filename, default_textures, textures_number, model_filename, default_models, models_number,
		minimum_gravity_influence_weight, calculations_epoch, true_julian);

	if (!object_creator)
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Can't create the objets creator", "Error", MB_OK);
		return false;
	}

	if (!object_creator->create_objects()) return false;

	light_ = new LightClass;
	if (!light_)
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Can't create light", "Error", MB_OK);
		return false;
	}

	light_->Initialize(object[star_object].getPosition_b(), { default_ambient_ilumination, default_ambient_ilumination, default_ambient_ilumination, },
	{ 0.0f, 0.0f, 0.0f });
	light_->SetLookAt({ 0.0f, 0.0f, 0.0f });
	light_->GenerateOrthoMatrix(20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

	aero_dynamics = new AeroDynamicsClass(object, *bullet_physics);
	if (!aero_dynamics) return false;

	GUI = new Gui_Class(*input, object, *bullet_physics, *aero_dynamics, camera->GetViewMatrix(), DirectX->GetProjectionMatrix(), DirectX->GetWorldMatrix(), total_objects);
	// Inicializamos La interfaz grafica del usuario yo ti amo bushoooo
	if (!GUI->Initialize_GUI(DirectX->GetOrthoMatrix(), win_system->getHWND(), screenWidth, screenHeight))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the User Interface.", "Error", MB_OK);
		return false;
	}

	if (!initialize_models())
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not create the 3D models.", "Error", MB_OK);
		return false;
	}

	if (!initialize_textures())
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not load the textures.", "Error", MB_OK);
		return false;
	}

	main_fps = new fpsClass;
	main_fps->StartCounter();
	main_fps->update();

	physics_engine_fps = new fpsClass;
	physics_engine_fps->StartCounter();
	physics_engine_fps->update();

	color = new color_class;
	if (!color) return false;

	stars = new StarClass;
	if (!stars->Initialize("objects/stars database/star_data146.txt", "objects/stars database/constellations.txt")) return false;

	auto_ = new AutoClass(object, *bullet_physics, camera->GetViewMatrix(), DirectX->GetProjectionMatrix(), DirectX->GetWorldMatrix(), total_objects, *aero_dynamics);
	if (!auto_) return false;

	script_ = new ScriptClass(object, *bullet_physics, auto_);
	if (!script_)
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Can't create the scripts manager", "Error", MB_OK);
		return false;
	}

	animation = new MovingPartsClass(object, *bullet_physics, auto_);
	if (!animation)
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Can't create the animations manager", "Error", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader->Initialize(win_system->getHWND())) return false;

	background_texture = new BitmapClass;
	if (!initialize_background_textures())
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the free textures.", "Error", MB_OK);
		return false;
	}

	//*
	// Create the texture shader object.
	instanced_shader = new InstanceShaderClass;
	if (!instanced_shader) return false;

	// Initialize the texture shader object.
	if (!instanced_shader->Initialize(win_system->getHWND()))
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not initialize the instance shader object.", "Error", MB_OK);
		return false;
	}
	//*/

	return true;
}

bool showMasterClass::run_show()
{
	//////////////////////////////////////////////
	//			One time initializations		//
	static bool init_ = false;
	if (!init_)
	{
		One_time_Initializations_pre();
		init_ = true;
	}
	//											//
	//////////////////////////////////////////////

	if (bullet_reload)
	{
		if (!BulletPhysics_ObjectManager()) return true; // resetea bullet
	}

	if (!threadize) astrophysics_engine();

	main_fps->update();
	dt_a = main_fps->get_dt();

	if (!calculos_generales()) return true; // calculos individuales
	if (!Update_all_objects()) return true; // calculos para todos los objetos
	
	if (!updateInput()) return true;

	if (!camera_manager())
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Camera error!", "Error", MB_OK);
		return true;
	}

	if (!render_manager())
	{
		DirectX->setWindowed();
		win_system->setWindowed();
		MessageBoxA(m_hwnd, "Could not render the graphics!", "Error", MB_OK);
		return true;
	}

	//////////////////////////////////////////////
	//			One time initializations		//
	static bool init_b = false;
	if (!init_b)
	{
		One_time_Initializations_post();
		Config.system_initialized = true;
		init_b = true;
	}
	//											//
	//////////////////////////////////////////////

	reset_scene();

	return false; // end show
}

bool showMasterClass::updateInput()
{
	input->update();
	float pressed_key_delay_rate = 0.3f;

	if (!Config.caption_on)
	{
		if (key_pressed == VK_ESCAPE) // process escape key actions
		{
			GUI_visible = true;
			GUI->escape_pressed = true;
		}

		if (GUI->kill_program)
		{
			toggle_engine(false);
			return false;
		}

		if (key_pressed == '0') // toggles GUI visible
		{
			key_pressed = 0;

			if (control == star_object)
			{
				control++;
				universe_scale = 1.0f / float(object[control].getScale());
				GUI_visible = true;
			}

			else GUI_visible = !GUI_visible;
		}

		if (key_pressed == 'f') // creates a copy of an object
		{
			freeze_aether_physics = !freeze_aether_physics;
		}


		if (key_pressed == 'B') // run bullet scenario
		{
			key_pressed = 0;

			int target = 65;

			vec3 x_ = GetAxisX(object[39].getTotalRotation());
			vec3 y_ = GetAxisY(object[39].getTotalRotation());
			vec3 z_ = GetAxisZ(object[39].getTotalRotation());
			vec3 dir_ = {500.0, 500.0, -1000.0};
			x_ = x_*dir_.x;
			y_ = y_*dir_.y;
			z_ = z_*dir_.z;
			dir_ = x_ + y_ + z_;

			object[39].setPosition(object[target].getPosition() + dir_);
			vec3 vel = normalized(object[target].getPosition() - object[39].getPosition());
			object[39].setVelocity(object[39].getVelocity() + vel*1000.);
		}


		if (key_pressed == 'C') // creates a copy of an object
		{
			key_pressed = 0;

			object.push_back(object[control]);

			// name the object
			for (int n = 0; n < total_objects; n++)
			{
				if (object[n].getOriginalName() == object[total_objects].getOriginalName())
				{
					object[total_objects].setCopyNumber(max(object[total_objects].getCopyNumber(), object[n].getCopyNumber() + 1));
				}
			}

			object[total_objects].setName(object[control].getOriginalName() + "_" + std::to_string(object[total_objects].getCopyNumber()));

			total_objects++;
		}

		if (key_pressed == 'D') // destroys an object
		{
			key_pressed = 0;

			// definimos el nombre del objeto orbitado
			std::string orbitado = object[control].get_orbiting_name();

			// establecemos el objeto como destruido
			object[control].setDestroyed(true);
			// creamos un array temporal
			std::vector<objectClass>new_object;
			// copiamos el array original en el array temporal, menos el objeto destruido
			for (int n = 0; n < total_objects; n++)
			{
				if (!object[n].getDestroyed()) new_object.push_back(object[n]);
			}

			// disminuimos la cantidad de objetos
			total_objects--;
			// vaciamos el array original
			object.clear();
			// copiamos el array temporal en el nuevo array original
			for (int n = 0; n < total_objects; n++) object.push_back(new_object[n]);

			// Nos aseguramos de que los numeros de objetos vinculados a cada objeto se cambien correctamente.
			// En caso de haber desaparecido tomar las medidas necesarias
			for (int n = 0; n < total_objects - 1; n++)
			{
				bool orbited_found = false;
				bool target_found = false;
				bool docked_to_found = false;

				for (int nn = 0; nn < total_objects - 1; nn++)
				{
					if (object[n].get_orbiting_name() == object[nn].getName())
					{
						object[n].setOrbiting(nn);
						orbited_found = true;
					}

					if (object[n].get_target_name() == object[nn].getName())
					{
						target_found = true;
						object[n].setTarget(nn);
					}

					if (object[n].get_docked_to_name() == object[nn].getName())
					{
						docked_to_found = true;
						object[n].setDockedTo(nn);
					}
				}

				// si el objeto orbitado desaparecio entonces que orbite la estrella
				if (!orbited_found)
				{
					object[n].setOrbiting(star_object);
					object[n].set_orbiting_name(object[star_object].getName());
				}

				// si el objetivo desaparecio entonces que no tenga objetivo
				if (!target_found)
				{
					object[n].setTarget(0);
					object[n].set_target_name("N/A");
				}

				// si el objeto al que estaba dockeado desaparecio entonces liberarlo
				if (!docked_to_found)
				{
					object[n].setDockedTo(-1);
					object[n].setTopAbsoluteParent(-1);
					object[n].setTopArtificialParent(-1);
					object[n].set_docked_to_name("N/A");
				}
			}

			// definimos el nuevo objeto observado como el objeto orbitado por el objeto destruido
			for (int n = 0; n < total_objects; n++)
			{
				if (object[n].getName() == orbitado)
				{
					control = n;
					break;
				}
			}
			
			// asignamos la nueva escala del universo
			universe_scale = 1.0f / float(object[control].getScale());
		}

		if (key_pressed == 'E') // creates an astronaut
		{
			key_pressed = 0;

			if (object[control].getTotalDockingPorts() > 0)
			{
				object.push_back(object[control]);
				control = total_objects;

				object[control].setName("Astronaut_" + std::to_string(total_objects + 1));
				object[control].setOriginalName(object[control].getName());
				object[control].setType(4);
				object[control].setMass(80);
				object[control].setEmptyMass(object[control].getMass());
				object[control].setOrbiting(object[control].getOrbiting());
				object[control].setEquatorialRadius(0.9);
				object[control].setPolarRadius(0.9);
				object[control].setScale(1.8);
				object[control].setModel(10);
				object[control].setRocketEngine(true);
				object[control].setMainEnginePower(200);
				object[control].setRCSpower(200);
				object[control].setHoverPower(200);
				object[control].setPosition(object[control].getPosition() + d3d_to_vec(object[control].getDockingPortWorldPosition(0) / universe_scale));

				object[control].delete_docking_ports();
				object[control].setTotalDockingPorts(0);

				universe_scale = 1.0f / float(object[control].getScale());
				switched_shadows = true;
				switched_object = true;

				total_objects++;
			}
		}

		if (key_pressed == 'A')// toggles maximum time acceleration or kills acceleration if any
		{
			key_pressed = 0;

			if (!max_out && time_acceleration > 1)
			{
				max_out = false;
			}

			else max_out = !max_out;

			if (!max_out)
			{
				time_acceleration = 1;
				toggle_engine(false);
			}

			else toggle_engine(true);
		}

		if (key_pressed == 'k') // kills object rotation
		{
			key_pressed = 0;

			object[control].setKillRotation(!object[control].getKillRotation());
		}

		if (key_pressed == ']') // increments time acceleration by twice the current one
		{
			key_pressed = 0;

			if (!max_out)
			{
				time_acceleration += time_acceleration;
			}

			toggle_engine(true);
		}

		if (key_pressed == '[') // decrements time acceleration by half the current one
		{
			key_pressed = 0;

			if (time_acceleration > 1.0f)
			{
				time_acceleration = int(ceil(time_acceleration / 2.0f));
			}
			else time_acceleration = 1;

			if (time_acceleration == 1) toggle_engine(false);
		}

		if (sync_ && time_acceleration == 1) toggle_sync(false);

		if (key_pressed == 'S') // toggles time synchronization
		{
			key_pressed = 0;

			toggle_sync(!sync_);
		}

		if (key_pressed == 'c') // toggle cockpit camera
		{
			key_pressed = 0;
			switched_camera = true;
			in_cockpit = !in_cockpit;
		}

		if (key_pressed == 'H') // *** DEBUG *** activates object's animations
		{
			key_pressed = 0;

			if (object[control].getHasAnimations())
			{
				if (object[control].animation[0].available)	object[control].animation[0].active = true;
			}
		}


		if (key_pressed == 'P') // save a screenshot
		{
			key_pressed = 0;

			ID3D11Texture2D* backBuffer = nullptr;
			HRESULT hr = DirectX->getSwapCHain()->GetBuffer(0, __uuidof(*backBuffer), (LPVOID*)&backBuffer);
			if (SUCCEEDED(hr))
			{
				bool exists_ = false;
				int seq_ = 1;
				std::string orig_filename_ = "screenshots/screensot";
				std::string filename_ = orig_filename_ + "_" + std::to_string(seq_) + ".jpg";

				std::ifstream infile(filename_);
				exists_ = infile.good();

				while (exists_)
				{
					seq_++;
					filename_ = orig_filename_ + "_" + std::to_string(seq_) + ".jpg";
					std::ifstream infile(filename_);
					exists_ = infile.good();
					infile.close();
				}

				hr = DirectX::SaveWICTextureToFile(D3D_context, backBuffer,
					GUID_ContainerFormatJpeg, model->string2widestring(filename_).c_str());
				backBuffer->Release();
			}
			if (FAILED(hr)) return false;
		}

		if (key_pressed == 'R') // save a depth map
		{
			key_pressed = 0;

			HRESULT hr;
			ID3D11Resource* backBuffer = nullptr;
			m_RenderTexture->GetShaderResourceView()->GetResource(&backBuffer);
			hr = DirectX::SaveWICTextureToFile(D3D_context, backBuffer,
				GUID_ContainerFormatJpeg, L"screenshots/depth_map.jpg");
			backBuffer->Release();
			if (FAILED(hr)) return false;
		}

		if (key_pressed == 'Q') // toggle SCS/RCS
		{
			key_pressed = 0;

			object[control].setAerodynamicControls(!object[control].getAerodynamicControls());

			if (!object[control].getAerodynamicControls()) GUI->setWindowVisible(6, true);
			else GUI->setWindowVisible(6, false);

		}

		// condiciones para hacer despegar a la nave
		if ((input->KeyPressed(DIK_R) || input->KeyPressed(DIK_PGUP) || input->KeyPressed(DIK_PGDN)
			|| input->KeyPressed(DIK_BACKSPACE) || input->KeyPressed(DIK_NUMPADPLUS) || input->KeyPressed(DIK_NUMPADMINUS)
			|| object[control].getHoverCurrentPower() > 0.0
			) && object[control].getLanded())
		{
			//MessageBoxA(win_system->getHWND(), "Hasta aqui todo bien","OK", MB_OK);
			if (object[control].getLanded() && object[control].getGroundSpeed() == 0.0) script_->undock_object(control);
		}

		if (object[control].getDockedTo() == -1) // comandos de movimiento de la nave, no se ejecutan si esta acoplada
		{
			if (key_pressed == 'L') // Landing gear deployment/ retraction
			{
				key_pressed = 0;

				object[control].setLandingGear(!object[control].getLandingGear());
			}


			if (!input->KeyPressed(DIK_LSHIFT) && !input->KeyPressed(DIK_RSHIFT) &&
				!input->KeyPressed(DIK_LCONTROL) && !input->KeyPressed(DIK_RCONTROL))
			{
				if (object[control].getLanded() && object[control].getLandingGear())
				{
					if (input->KeyPressed(DIK_W)) auto_->RotateX(control, aero_dynamics->getSCS_maneuverability(control, dt_a) * dt_a);
					if (input->KeyPressed(DIK_S)) auto_->RotateX(control, -aero_dynamics->getSCS_maneuverability(control, dt_a) * dt_a);

					if (input->KeyPressed(DIK_A)) auto_->RotateY(control, aero_dynamics->getSCS_maneuverability(control, dt_a) * 3 * dt_a);
					if (input->KeyPressed(DIK_D)) auto_->RotateY(control, -aero_dynamics->getSCS_maneuverability(control, dt_a) * 3 * dt_a);
				}

				else
				{
					if (!object[control].getAerodynamicControls()) // if aerodynamic controls are not on then use the RCS (Reaction Control System)
					{
						if (input->KeyPressed(DIK_UP)) auto_->ApplyTranslation(3, -aero_dynamics->getRCS_maneuverability(control), 'z', control);
						if (input->KeyPressed(DIK_DOWN)) auto_->ApplyTranslation(3, aero_dynamics->getRCS_maneuverability(control), 'z', control);

						if (input->KeyPressed(DIK_RIGHT)) auto_->ApplyTranslation(3, -aero_dynamics->getRCS_maneuverability(control), 'x', control);
						if (input->KeyPressed(DIK_LEFT)) auto_->ApplyTranslation(3, aero_dynamics->getRCS_maneuverability(control), 'x', control);

						if (input->KeyPressed(DIK_F)) auto_->ApplyTranslation(3, -aero_dynamics->getRCS_maneuverability(control), 'y', control);
						if (input->KeyPressed(DIK_R)) auto_->ApplyTranslation(3, aero_dynamics->getRCS_maneuverability(control), 'y', control);

						if (input->KeyPressed(DIK_S)) bullet_physics->applyTorqueX(control, -aero_dynamics->getRCS_maneuverability(control));
						if (input->KeyPressed(DIK_W)) bullet_physics->applyTorqueX(control, aero_dynamics->getRCS_maneuverability(control));

						if (input->KeyPressed(DIK_D)) bullet_physics->applyTorqueY(control, -aero_dynamics->getRCS_maneuverability(control));
						if (input->KeyPressed(DIK_A)) bullet_physics->applyTorqueY(control, aero_dynamics->getRCS_maneuverability(control));

						if (input->KeyPressed(DIK_E)) bullet_physics->applyTorqueZ(control, -aero_dynamics->getRCS_maneuverability(control));
						if (input->KeyPressed(DIK_Q)) bullet_physics->applyTorqueZ(control, aero_dynamics->getRCS_maneuverability(control));
					}

					else if (object[control].getInAtmosphere()) // if aerodynamic controls are on then use the SCS (Surface Control System)
					{
						if (input->KeyPressed(DIK_W)) auto_->RotateX(control, aero_dynamics->getSCS_maneuverability(control, dt_a) * dt_a);
						if (input->KeyPressed(DIK_S)) auto_->RotateX(control, -aero_dynamics->getSCS_maneuverability(control, dt_a) * dt_a);

						if (input->KeyPressed(DIK_A)) auto_->RotateZ(control, aero_dynamics->getSCS_maneuverability(control, dt_a) * 3 * dt_a);
						if (input->KeyPressed(DIK_D)) auto_->RotateZ(control, -aero_dynamics->getSCS_maneuverability(control, dt_a) * 3 * dt_a);
					}
				}


				if (input->KeyPressed(DIK_NUMPADPLUS)) object[control].setMEcurrentPower(min(100.0, object[control].getMECurrentPower() + 10.0*dt_a));
				if (input->KeyPressed(DIK_NUMPADMINUS)) object[control].setMEcurrentPower(max(0.0, object[control].getMECurrentPower() - 10.0*dt_a));
				if (input->KeyPressed(DIK_BACKSPACE)) object[control].setMEcurrentPower(0.0);

				if (input->KeyPressed(DIK_PGUP)) object[control].setHoverCurrentPower(min(100.0, object[control].getHoverCurrentPower() + 10.0*dt_a));
				if (input->KeyPressed(DIK_PGDN)) object[control].setHoverCurrentPower(max(0.0, object[control].getHoverCurrentPower() - 10.0*dt_a));
			}

			else if (input->KeyPressed(DIK_LCONTROL))
			{
				double main_speed_ = object[control].getMainEnginePower();
				double hover_speed_ = object[control].getHoverPower();
				
				if (!object[control].getInBullet())
				{
					main_speed_ = main_speed_ / object[control].getMass()*dt_a;
					hover_speed_ = hover_speed_ / object[control].getMass()*dt_a;
				}

				if (input->KeyPressed(DIK_UP))
				{
					object[control].setQuickMeBurst(true);
					auto_->ApplyTranslation(1, -main_speed_, 'z', control);
				}
				else object[control].setQuickMeBurst(false);

				if (input->KeyPressed(DIK_R))
				{
					object[control].setQuickHoverBurst(true);
					auto_->ApplyTranslation(2, hover_speed_, 'y', control);
				}
				else object[control].setQuickHoverBurst(false);

				if (input->KeyPressed(DIK_NUMPADPLUS)) object[control].setMEcurrentPower(min(100.0, object[control].getMECurrentPower() + dt_a));
				if (input->KeyPressed(DIK_NUMPADMINUS)) object[control].setMEcurrentPower(max(0.0, object[control].getMECurrentPower() - dt_a));
				if (input->KeyPressed(DIK_BACKSPACE)) object[control].setMEcurrentPower(100.0);

				if (input->KeyPressed(DIK_PGUP)) object[control].setHoverCurrentPower(min(100.0, object[control].getHoverCurrentPower() + dt_a));
				if (input->KeyPressed(DIK_PGDN)) object[control].setHoverCurrentPower(max(0.0, object[control].getHoverCurrentPower() - dt_a));
			}
		}

		if (Config.command == "dock")
		{
			Config.command = "";

			object[control].setDockReady(true);
			if (object[control].getDockedTo() != -1) script_->undock_object(control);
			else script_->dock_object(control, object[control].getTarget(), 20.f);

		}

		if (Config.command == "stage")
		{
			Config.command = "";
			if (!script_->stages_manager(control)) return false;
		}

		if (key_pressed == 'y') // toggles camera rotation behavior
		{
			key_pressed = 0;
			camera->setOrientar(!camera->getOrientar());
			camera->setPreferTargetYAxisOrbiting(!camera->getPreferTargetYAxisOrbiting());
		}
	}

	if (int(input->GetMouseSate().z) != 0)
	{
		static float former_universe_scale = 0.0;
		former_universe_scale = universe_scale;
		if (!in_cockpit)
		{
			Config.changed_scale = true;
			if (input->GetMouseSate().z > 0)
			{
				// minimun distance the camera can get close to the observed object, so it won't go through it
				float minimo_ = 1.0f / (float(object[control].getScale()) / 2.1f);
				if (universe_scale < minimo_) universe_scale = min(universe_scale*1.1f, minimo_);
			}

			else
			{
				// maximum distance the camera can get away from the sun, so the sun can still be visible as a light point among the stars
				float maximo_ = 1e-14f;
				if (universe_scale>maximo_) universe_scale = max(universe_scale*0.9f, maximo_);
			}
		}

		// si la camara entra bajo tierra al alejarla entonces acercarla de vuelta para evitar ver bajo la tierra
		/*
		if (length_f(camera->GetPosition()- object[object[control].getOrbiting()].getPosition_b()) <=
		(object[object[control].getOrbiting()].getScale_b() / 2.0) + 1.f * universe_scale && !in_cockpit)
		{
		universe_scale = former_universe_scale*1.1f;
		}
		//*/
	}

	if (input->MouseButtonDown(1)) camera->setActivateRotation(true);
	else camera->setActivateRotation(false);

	if (input->MouseButtonDown(2)) camera->setRotarCamara(true);
	else camera->setRotarCamara(false);

	return true;
}

bool showMasterClass::sunlit(vec3 raydir, vec3 rayorig, vec3 pos, float rad)
{
	float a = float(raydir* raydir);
	float b = float(raydir* ((rayorig - pos) * 2.0f));
	float c = float((pos* pos) + (rayorig* rayorig) - 2.0f * (rayorig* pos) - rad * rad);
	float D = b * b + (-4.0f) * a * c;
	float distance = 0.0f;
	vec3 hitpoint = { 0.0, 0.0, 0.0 };

	if (D < 0.0f) return true;

	if (length(rayorig - d3d_to_vec(light_->GetPosition())) < length(pos - d3d_to_vec(light_->GetPosition()))) return true;

	return false;
}

bool showMasterClass::render_general_objects()
{
	ready_to_render = true;

	bool has_transparencies = false;

	if (!RenderSceneToTexture()) return false;

	DirectX->ClearScreen();

	render_background_textures();

	//*
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																																		//

	//																																		//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//*/

	for (int n = 3; n < total_objects; n++)
	{
		if (object[n].getVisible() && exclude_background_objects(n))
		{
			DirectX->setWorldMatrix(object[n].getWorld());
			set_special_objects_properties(n);
			set_near_and_far_points(n);	// render huge distant objects correctly

			if (n != star_object)
			{
				model[object[n].getModel()].set_buffers();

				// Set the shader
				if (!set_shader_to_use(n)) return false;

				for (int nn = 0; nn < model[object[n].getModel()].GetMaterialCount(); nn++)
				{
					if (model[object[n].getModel()].getTransparency(nn) == 1)
					{
						set_texture_array(n, nn); // set the textures to be sent to the shader

						set_shader_parameters(n, nn);

						// render the dynamic sphere and landing pad without indices
						if (object[n].getModel() == 8 || object[n].getModel() == 9)	DirectX->GetDeviceContext()->Draw(model[object[n].getModel()].GetVertexCount(), 0);

						else
						{
							if (model[object[n].getModel()].GetMaterialName(nn) == "landing_gear")
							{
								if (object[n].getLandingGear()) DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(nn),
									model[object[n].getModel()].GetIndexStart(nn), 0);
							}

							else if (model[object[n].getModel()].GetMaterialName(nn) == "Windshield" && in_cockpit)
							{
								// do not draw the windshield from the inside
							}

							else DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(nn),
								model[object[n].getModel()].GetIndexStart(nn), 0);
						}
					}
					else has_transparencies = true;
				}

				if (has_transparencies)
				{
					DirectX->enable_transparencies();
					for (int nn = 0; nn < model[object[n].getModel()].GetMaterialCount(); nn++)
					{
						if (model[object[n].getModel()].getTransparency(nn) < 1)
						{
							set_texture_array(n, nn); // set the textures to be sent to the shader

							set_shader_parameters(n, nn);

							if (object[n].getModel() == 8 || object[n].getModel() == 9)	DirectX->GetDeviceContext()->Draw(model[object[n].getModel()].GetVertexCount(), 0);
							else
							{
								if (model[object[n].getModel()].GetMaterialName(nn) == "landing_gear")
								{
									if (object[n].getLandingGear()) DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(nn),
										model[object[n].getModel()].GetIndexStart(nn), 0);
								}

								else if (model[object[n].getModel()].GetMaterialName(nn) == "Windshield" && in_cockpit)
								{
									// do not draw the windshield from the inside
								}

								else DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(nn),
									model[object[n].getModel()].GetIndexStart(nn), 0);
							}
						}
					}

					has_transparencies = false;
				}
			}
		}
	}

	if (!render_3D_lines()) return false;

	// dibujamos las columnas de combustion y humo de los motores
	if (!render_engine_particles()) return false;

	if (GUI_visible)
	{
		if (!render_GUI())
		{
			DirectX->setWindowed();
			win_system->setWindowed();
			MessageBoxA(m_hwnd, "Could not render the GUI", "Error", MB_OK);
			return false;
		}
	}

	if (ready_to_render && !sync_) DirectX->PresentScene();

	return true;
}

bool showMasterClass::initialize_models()
{
	// Create the models
	model = new modelClass[models_number];
	if (!model) return false;

	if (!model[0].load_model("models/spheres/sphere_0.txt")) return false;
	if (!model[1].load_model("models/spheres/sphere_1.txt")) return false;
	if (!model[2].load_model("models/spheres/sphere_2.txt")) return false;
	if (!model[3].load_model("models/spheres/sphere_3.txt")) return false;
	if (!model[4].load_model("models/spheres/sphere_4.txt")) return false;
	if (!model[5].load_model("models/spheres/sphere_5.txt")) return false;
	if (!model[6].load_model("models/spheres/sphere_6.txt")) return false;
	if (!model[7].load_model("models/spheres/shadow_receiver.txt")) return false;
	if (!model[8].create_dynamic_sphere("models/spheres/dynamic_sphere.txt", 7, 0)) return false; // the dynamic sphere
	if (!model[9].create_dynamic_landing_pad("models/spheres/dynamic_sphere.txt")) return false; // the dynamic landing pad
	if (!model[10].load_model("models/EMU/emu.txt")) return false; // the astronaut

	return true;
}

bool showMasterClass::initialize_textures()
{
	texture = new textureClass[textures_number];
	if (!texture) return false;

	if (!texture[0].load_texture("textures/white.png")) return false;
	if (!texture[1].load_texture("textures/high_res/skymap3.jpg")) return false;

	return true;
}

bool showMasterClass::update_model(int obj_)
{
	// load the model of the specified object only
	for (UINT n = 0; n < model_filename.size(); n++)
	{
		if (object[obj_].getModel() == n + default_models && !model[object[obj_].getModel()].getLoaded())
		{
			if (object[obj_].getType() != 8) // if object is not a ring system
			{
				if (!model[n + default_models].load_model(model_filename[n])) return false;
			}

			else // object is a ring system, create a ring model for it
			{
				if (!model[n + default_models].create_rings(100, object[obj_].getInnerRing(), object[obj_].getOuterRing(), 0.0f, 360.0f, object[obj_].getRingsTexture())) return false;
			}
		}
	}

	return true;
}

bool showMasterClass::update_texture(int obj_)
{
	// load the textures of the specified object only
	for (UINT n = 0; n < texture_filename.size(); n++)
	{
		if ((object[obj_].getTexture1() == n + default_textures ||
			object[obj_].getTexture2() == n + default_textures ||
			object[obj_].getTexture3() == n + default_textures) &&
			!texture[n + default_textures].getLoaded())
		{
			if (!texture[n + default_textures].load_texture(texture_filename[n])) return false;
		}
	}

	return true;
}

void showMasterClass::transform_object(int objeto)
{
	update_model_used(objeto);

	if (objeto != 3)
	{
		// Create the local transformation matrix
		Matrix escalar, rotar, trasladar;
		float esc_ = object[objeto].getScale_b();
		if (object[objeto].getModel() == 8) esc_ /= 2;

		if (object[objeto].getPolarRadius() < object[objeto].getEquatorialRadius()) escalar = DirectX::XMMatrixScaling(esc_, esc_* (1.0f - object[objeto].getFlattening()), esc_);
		else if (object[objeto].getPolarRadius() < object[objeto].getEquatorialRadius()) escalar = DirectX::XMMatrixScaling(esc_* (1.0f - object[objeto].getFlattening()), esc_, esc_);

		else escalar = DirectX::XMMatrixScaling(esc_, esc_, esc_);
		rotar = DirectX::XMMatrixRotationQuaternion(quat_to_d3dquat(~object[objeto].getTotalRotation()));
		Vector3 tras_ = object[objeto].getPosition_b();
		trasladar = DirectX::XMMatrixTranslation(tras_.x, tras_.y, tras_.z);

		object[objeto].setWorld(escalar*rotar*trasladar);
	}

	// *** DEBUG *** very expensive computation done every frame!!! please find a cheaper one
	else if (camera_altitude <= landing_pad_view_altitude) model[9].update_dynamic_landing_pad_VertexBuffer(tf, object);

}

void showMasterClass::set_texture_array(int n, int nn)
{
	// si la textura es -1 se usa la textura default del modelo
	if (object[n].getTexture1() < 0) texture_array[0] =
		model[object[n].getModel()].Get_texture_pool(model[object[n].getModel()].Get_diffuse_map(nn));
	else texture_array[0] = texture[object[n].getTexture1()].get_Texture();

	if (object[n].getTexture2() < 0) texture_array[1] =
		model[object[n].getModel()].Get_texture_pool(model[object[n].getModel()].Get_light_map(nn));
	else texture_array[1] = texture[object[n].getTexture2()].get_Texture();

	if (object[n].getTexture3() < 0) texture_array[2] =
		texture[0].get_Texture();
	else texture_array[2] = texture[object[n].getTexture3()].get_Texture();
}

void showMasterClass::set_near_and_far_points(int n)
{
	if (n == 0) DirectX->setNearFar(1.f, 2e7f);

	else if (in_cockpit && object[n].getType() == 4) DirectX->setNearFar(0.01f, 2e7f);

	else if (((object[n].getType() == 2 || object[n].getType() == 8) && camera_in_atmosphere > 1)
		|| (object[n].getType() == 3 && !inside_range(n, 100.f)))
	{
		if (object[n].getType() == 3)
		{
			if (camera_in_atmosphere > 1)
			{
				int ob_ = object[n].getOrbiting();
				float near_ = (object[ob_].getDistanceToCamera() - object[ob_].getScale_b() / 2.f) / 4.f;
				DirectX->setNearFar(near_, 1e20f);
			}

			else
			{
				float near_ = (object[n].getDistanceToCamera() - object[n].getScale_b() / 2.f) / 4.f;
				DirectX->setNearFar(near_, 1e20f);
			}
		}

		else
		{
			float near_ = (object[n].getDistanceToCamera() - object[n].getScale_b() / 2.f) / 4.f;
			DirectX->setNearFar(near_, 1e20f);
		}
	}

	else if ((object[n].getType() == 7) && camera_in_atmosphere > 1)
	{
		float near_ = (object[n].getDistanceToCamera() - object[n].getScale_b() / 2.f) / 2.f;
		DirectX->setNearFar(near_, 1e20f);
	}

	//else if ((object[n].getType() == 2 || object[n].getType() == 5 || object[n].getType() == 7)
	//	&& distances_f(camera->GetPosition(),object[n].getPosition_b())>50000.f) DirectX->setNearFar(100.f, 1e10f);

	else DirectX->setNearFar(0.1f, 2e7f);

	shadows_manager();

	return;
}

void showMasterClass::set_special_objects_properties(int n)
{
	if (n == 0 || n == 2 || object[n].getType() == 5 || (object[n].getType() == 7 && under_clouds < 1.0f)) // starfield, "inside-out" atmosphere or clouds layer from below
	{
		DirectX->set_inversed_render(true);
		inversed_render_ = true;
	}

	else if (inversed_render_) DirectX->set_inversed_render(false);

	return;
}

void showMasterClass::start_engine()
{
	physics_engine = std::thread(&showMasterClass::astrophysics_engine, this);
}

void showMasterClass::stop_engine()
{
	cut_engine = true;
	physics_engine.join(); // wait for the thread to finish.
	physics_engine.~thread(); // thread destructor, calls "terminate" on it
}

void showMasterClass::astrophysics_engine()
{
	double distancia_entre_cuerpos = 0.0;
	double Fuerza_de_gravedad = 0.0;
	double aceleracion = 0.0;
	vec3 normal_vector = { 0.0, 0.0, 0.0 };
	static double dt_ = 0.0;
	static vec3 former_position = { 0.0, 0.0, 0.0 };

	if (!threadize) cut_engine = false;
	while (!cut_engine)
	{
		//time_acceleration = 0;
		physics_engine_fps->update();

		dt_ = physics_engine_fps->get_dt()*time_acceleration;
		//dt_b = physics_engine_fps->get_dt();
		dt_b = float(dt_);

		if (dt_ > 1.0) // limit the time_acceleration multiplication to 1 to avoid data loss, this limit uses the maximum cpu power available for this thread
			// without sacrificing precision
		{
			//if (!max_out) time_acceleration -= 10;
			//else time_acceleration -= 2;
			time_acceleration -= 10;
			if (time_acceleration < 1.0f) time_acceleration = 1;
			dt_ = 1.0;
		}

		else if (max_out)
		{
			time_acceleration++; // flag to maximize the time acceleration
			if (sync_)
			{
				if (true_julian>full_date) time_acceleration = min(time_acceleration, int(1.0 + (true_julian - full_date) * 1e6));
				else time_acceleration = 1;
			}
		}

		for (int n = 0; n < total_objects; n++)
		{
			if (object[n].getType() == 4) auto_->update(n); // updates the autopilot

			// Leave the following objects out of the equations:
			// type 0: unaffected objects
			// type 5: inside-out atmospheres
			// type 6: forward atmospheres
			// type 7: clouds
			// type 8: planetary rings
			// type 9: the landing pad
			// docked objects

			if (object[n].getAffects() != 0 && object[n].getType() != 5 && object[n].getType() != 6 && object[n].getType() != 7
				&& object[n].getType() != 8 && object[n].getType() != 9 && object[n].getDockedTo() == -1)
			{
				for (int nn = 0; nn < total_objects; nn++)
				{
					// Leave the following objects out of the equations:
					// type 0: unaffected objects
					// type 5: inside-out atmospheres
					// type 6: forward atmospheres
					// type 7: clouds
					// type 8: planetary rings
					// type 9: the landing pad
					// docked objects

					if (object[nn].getAffects() == 1 && object[nn].getType() != 5 && object[nn].getType() != 6 && object[nn].getType() != 7
						&& object[nn].getType() != 8 && object[nn].getType() != 9 && object[nn].getDockedTo() == -1)
					{
						// calculamos la distancia entre los cuerpos
						distancia_entre_cuerpos = distances(object[n].getPosition(), object[nn].getPosition());

						if (distancia_entre_cuerpos != 0.0) // si no es una singularidad, lo que causaria un error de calculos
						{
							// formula de Newton para hallar la fuerza de gravedad entre dos cuerpos
							Fuerza_de_gravedad = GRAVITATIONAL_CONSTANT*((object[n].getMass() * object[nn].getMass()) / (distancia_entre_cuerpos * distancia_entre_cuerpos));

							// formula para definir la aceleracion de un cuerpo hacia otro de acuerdo a su masa y la fuerza de gravedad entre ambos
							aceleracion = Fuerza_de_gravedad / object[n].getMass();

							if (object[n].getType() == 4 && nn == object[n].getOrbiting())
							{
								object[n].setG_Acceleration(aceleracion);
							}

							// definimos el vector de posicion normalizado (unitario) entre los cuerpos
							normal_vector = normalized(object[nn].getPosition() - object[n].getPosition());

							// movemos el cuerpo de acuerdo al vector normalizado y lo multiplicamos por la aceleracion, guardamos el resultado
							// en la variable provisoria "TotalMovement"
							object[n].setTotalMovement(object[n].getTotalMovement() + normal_vector * aceleracion);
						}
					}
				}
			}
		}

		object[1].Rotate({ 0.0, 0.0, 0.0, 1.0 }, dt_); // giramos el objeto-reloj

		for (int n = 0; n < total_objects; n++)
		{
			// Leave the following objects out of the equations:
			// type 0: unaffected objects

			if (object[n].getAffects() != 0)
			{
				// If the object is not docked then rotate it freely
				// *** DEBUG *** que exactamente hace la siguiente linea?
				if (object[n].getDockedTo() == -1) object[n].Rotate({ 0.0, 0.0, 0.0, 1.0 }, dt_);

				// Leave the following objects out of the equations:
				// type 5: inside-out atmospheres
				// type 6: forward atmospheres
				// type 7: clouds
				// type 8: planetary rings
				// type 9: the landing pad
				// docked objects

				if (object[n].getType() != 5 && object[n].getType() != 6 && object[n].getType() != 7
					&& object[n].getType() != 8 && object[n].getType() != 9 && object[n].getDockedTo() == -1)
				{
					// movemos los objetos usando el valor acumulado en MovimientoTotal multiplicado por el tiempo delta.
					// tiempo delta es el tiempo en segundos que le llevo al cpu calcular una frame, o sea volver a este mismo lugar despues de
					// realizar todos los calculos

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					//																																	//
					//* // Aqui aplicamos el arrastre aerodinamico al movimiento total que las fuerzas gravitacionales ejercen sobre el vehiculo
					// si esta dentro de una atmosfera
					if (object[n].getType() == 4 && object[n].getDockedTo() == -1 &&
						length(object[n].getPosition() - object[object[n].getOrbiting()].getPosition()) <= (object[object[n].getOrbiting()].getScale() / 2)*object[object[n].getOrbiting()].getAtmosphereRatio())

					{
						aero_dynamics->ModifyTotalMovement(n, object[n].getTotalMovement()*dt_, dt_);
						object[n].setVelocity(object[n].getVelocity() + object[n].getTotalMovement());
					}

					else//*/
						//																																	//
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						object[n].setVelocity(object[n].getVelocity() + object[n].getTotalMovement() *dt_);

					// *** DEBUG *** inprovised crash/landing check and handling
					// this only checks for collissions with a perfect sphere, please change to handle ellipsoids!
					// like check for altitude using the latitude/longitude/altitude algorithm


					if (n == control && object[n].getDockedTo() == -1)
					{

						if (!object[n].getLandingGear())
						{
							if (length((object[n].getPosition() + object[n].getVelocity() * dt_) - object[object[n].getOrbiting()].getPosition()) >=
								object[object[n].getOrbiting()].getScale() / 2.0)

								object[n].setPosition(object[n].getPosition() + object[n].getVelocity() * dt_);

							else crash_manager(n);
						}

						else
						{
							if (length((object[n].getPosition() + object[n].getVelocity() * dt_) - object[object[n].getOrbiting()].getPosition()) >=
								object[object[n].getOrbiting()].getScale() / 2.0 - object[n].getGroundSensorDistance())
							{
								object[n].setPosition(object[n].getPosition() + object[n].getVelocity() * dt_);
								object[n].setLanded(false);
							}


							else
							{
								landing_manager(n, float(dt_));
							}
						}
					}

					else object[n].setPosition(object[n].getPosition() + object[n].getVelocity() * dt_);

					// incluir calculos aerodinamicos si el objeto esta dentro de una atmosfera
					if (object[n].getType() == 4 && object[n].getDockedTo() == -1)
					{
						object[n].UpdateCustomData(object[object[n].getOrbiting()].getPosition(),
							object[object[n].getOrbiting()].getScale(), object[object[n].getOrbiting()].getTotalRotation());

						if (length(object[n].getPosition() - object[object[n].getOrbiting()].getPosition()) <= (object[object[n].getOrbiting()].getScale() / 2)*object[object[n].getOrbiting()].getAtmosphereRatio())
						{
							object[n].setInAtmosphere(true);
							aero_dynamics->apply_aerodynamics(n, dt_);
						}

						else object[n].setInAtmosphere(false);
					}					

					object[n].setTotalMovement({ 0.0, 0.0, 0.0 });  // reseteamos el valor provisorio
				}

				else if (object[n].getDockedTo() == -1) // object is an atmosphere, clouds layer, landing pad or planetary ring
				{
					object[n].setPosition(object[object[n].getOrbiting()].getPosition());
				}
			}
		}

		for (int n = 0; n < total_objects; n++)
		{
			// actualizamos la rotacion de los objetos acoplados usando quaternios de acuerdo a la rotacion de su objeto padre
			if (object[n].getDockedTo() != -1)
			{
				object[n].Rotate(object[object[n].getDockedTo()].getTotalRotation(), dt_);

				// seteamos la posicion a la posicion original mas la rotacion del objeto padre
				object[n].setPosition(object[object[n].getDockedTo()].getPosition() +
					vector_rotation_by_quaternion(object[n].getDocked_position(), ~object[object[n].getDockedTo()].getTotalRotation()));

				// seteamos la velocidad a la diferencia entre la posicion anterior y la presente
				//vec3 current_vel = (object[n].getPosition() - former_position) / dt_;
				//object[n].setVelocity(current_vel);
				//former_position = object[n].getPosition();
			}
		}

		// movemos el objeto controlado al centro del universo y todos los demas de acuerdo a este, para que DirectX los dibuje con precision
		// para esto usamos dos posiciones y velocidades, las originales no las modificamos para que los calculos de precision doble no se vean afectados,
		// las otras son las que usa DirectX

		// primero actualizamos la posicion y escala de todos los objetos en variables provisorias para evitar artefactos
		// en el render cuando acercamos o alejamos la camara

		vec3 restar_posicion = object[control].getPosition();
		aether_velocity = object[control].getVelocity();

		for (int n = 0; n < total_objects; n++)
		{
			if (object[n].getAffects() != 0)
			{
				object[n].setPosition_b(vec_to_d3d(object[n].getPosition() - restar_posicion) * universe_scale);
				object[n].setPosition_bullet(vec_to_d3d(object[n].getPosition() - restar_posicion));
				object[n].setScale_b(float(object[n].getScale()) * universe_scale);

				object[n].setVelocity_b(vec_to_d3d(object[n].getVelocity() - aether_velocity));
			}
		}

		if (Config.system_initialized) bullet_physics->step_simulation();		

		// luego actualizamos la posicion y escala a ser utilizadas por DirectX solo si no se acaba de cambiar la distancia 
		// de la camara, de otra forma esperar la proxima pasada para que todos los objetos ya esten actualizados

		// si usamos multiprecision
		/*
		for (int n = 0; n < total_objects; n++)
		{
		if (!changed_scale && object[n].getAffects() != 0)
		{
		object[n].setPosition_b(object[n].getPosition_c());
		object[n].setScale_b(object[n].getScale_c());
		}
		}
		*/

		if (!threadize) cut_engine = true;
	}
}

bool showMasterClass::set_shader_to_use(int object_)
{
	if (object[object_].getType() != 5 && object[object_].getType() != 6)
	{
		if (!Basic_shader->set_basic_shader()) return false;
	}

	else
	{
		if (!Atmosphere_shader->set_atmosphere_shader()) return false;
	}

	return true;
}

bool showMasterClass::set_shader_parameters(int object_, int material_)
{
	Vector3 diffuse_color = { 0.0f, 0.0f, 0.0f };
	float ambient_ilumination = default_ambient_ilumination;

	// simulamos la iluminacion que se refleja sobre el objeto desde el lado iluminado del cuerpo orbitado
	if (object[object_].getType() == 4 || object[object_].getType() == 10 || object[object_].getType() == 11)
	{
		Vector3 vector1, vector2;
		float ilum = 0;
		vector1 = object[object_].getPosition_b() - object[object[object_].getOrbiting()].getPosition_b();
		vector2 = object[star_object].getPosition_b() - object[object[object_].getOrbiting()].getPosition_b();
		vector1.Normalize();
		vector2.Normalize();
		ilum = vector1.Dot(vector2);

		ambient_ilumination = max(default_ambient_ilumination / 1.5f, ilum / 4.0f);
	}


	if (material_check >= 0 && material_check == material_ && object_ == control) diffuse_color = Vector3{ 1.0f, 0.0f, 0.0f }; // paint materials in red to debug
	else diffuse_color = model[object[object_].getModel()].getDiffuseColor(material_);

	if (object[object_].getType() != 5 && object[object_].getType() != 6)
	{
		if (object[object_].getType() == 7 || object[object_].getType() == 8) DirectX->enable_transparencies();

		if (!Basic_shader->Set_Basic_shader_parameters(DirectX->GetWorldMatrix(),
			camera->GetViewMatrix(), DirectX->GetProjectionMatrix(),
			light_->GetViewMatrix(), light_->GetOrthoMatrix(),
			light_->GetPosition(), max(object[object_].getIlumination(),
			max(model[object[object_].getModel()].getIlumination(material_), ambient_ilumination)),
			camera->GetPosition(), model[object[object_].getModel()].getTransparency(material_), diffuse_color,
			model[object[object_].getModel()].getSpecularPower(material_), texture_array,
			m_RenderTexture->GetShaderResourceView(),
			max(object[object_].getHasLightMap(), model[object[object_].getModel()].Get_has_light_map(material_)),
			max(object[object_].getHasNightMap(), model[object[object_].getModel()].Get_has_night_map(material_)),
			float(object_), float(object[object_].getScale_b()), object[object_].getHasAtmosphere(),
			object[object[object_].getOrbiting()].getPosition_b(), object[object[object_].getOrbiting()].getScale_b() / 2,
			object[object_].getPosition_b(), object[object_].getScale_b() / 2,
			float(object[object_].getType()), object[object_].getHasSpecularMap(),
			float(distances(d3d_to_vec(camera->GetPosition()), d3d_to_vec(object[star_object].getPosition_b())) / universe_scale),
			object[object_].getSunlit(),
			object[object_].getSunsetColor(),
			camera_in_atmosphere,
			object[object_].getDayLightColor(), 0.0f)
			) return false;
	}

	else
	{
		DirectX->enable_transparencies();
		if (!Atmosphere_shader->Set_Atmosphere_shader_parameters(DirectX->GetWorldMatrix(),
			camera->GetViewMatrix(), DirectX->GetProjectionMatrix(), light_->GetPosition(),
			(object[object[object_].getOrbiting()].getScale_b() / 2)*object[object[object_].getOrbiting()].getAtmosphereRatio(),
			camera->GetPosition(), model[object[object_].getModel()].getTransparency(material_),
			object[object[object_].getOrbiting()].getPosition_b(),
			model[object[object_].getModel()].getSpecularPower(material_),
			texture_array, object[object[object_].getOrbiting()].getScale_b() / 2,
			float(object_), float(object[object_].getType()), camera_in_atmosphere,
			object[object_].getSunsetColor(),
			float(distances(d3d_to_vec(camera->GetPosition()), d3d_to_vec(object[star_object].getPosition_b())) / universe_scale),
			object[object_].getDayLightColor(), object[object_].getAtmosphereThickness())) return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										Funcion para checkear si la distancia entre un objeto y la camara											//
//											es mayor o menor a la cantidad especificada de dichos objetos											//
inline bool showMasterClass::inside_range(int body_, const float body_count_)
{
	if (object[body_].getDistanceToCamera() <= object[body_].getScale_b() * body_count_) return true;

	return false;
}
//																																					//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//							Asignar la complejidad del modelo de acuerdo a su distancia de la camara y su tamanho									//
void showMasterClass::update_model_used(int body_)
{
	if ((object[body_].getModel() >= 0 && object[body_].getModel() <= 6) && object[body_].getAffects() != 0) // el rango de los modelos esfericos 0-6
	{
		if (inside_range(body_, 1.0f)) object[body_].setModel(6); // (32768 triangles)
		else if (inside_range(body_, 3.0f)) object[body_].setModel(5); // (8192 triangles)
		else if (inside_range(body_, 9.0f)) object[body_].setModel(4); // (2048 triangles)
		else if (inside_range(body_, 27.0f)) object[body_].setModel(3); // (512 triangles )
		else if (inside_range(body_, 250.0f)) object[body_].setModel(2); // (128 triangles )
		else if (inside_range(body_, 500.0f)) object[body_].setModel(1); // (32 triangles )
		else object[body_].setModel(0); // (8 triangles )
	}
}
//																																					//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool showMasterClass::render_GUI()
{

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														Render the User Interface																	//
	DirectX->enable_transparencies();
	DirectX->TurnZBufferOff();
	DirectX->set_inversed_render(false);

	// update the UI every n seconds to increase performance
	if (seconds_since_last_messages_update > update_messages_interval)
	{
		if (!GUI->update_ventanas(time_acceleration, control, full_date)) return false;
		GUI->update_buttons();
		seconds_since_last_messages_update = 0.0f;
	}

	if (!GUI->render()) return false;
	if (!render_projected_text()) return false;

	DirectX->TurnZBufferOn();
	DirectX->disable_transparencies();
	//																																					//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

void showMasterClass::toggle_engine(bool engine_state_)
{
	if (engine_state_)
	{
		if (!threadize)
		{
			threadize = true;
			cut_engine = false;
			start_engine();
		}
	}

	else
	{
		if (threadize)
		{
			threadize = false;
			cut_engine = true;
			stop_engine();
		}
	}
}

bool showMasterClass::calculos_generales()
{
	light_->SetPosition(object[star_object].getPosition_b());
	seconds_since_last_messages_update += dt_a; // temporizador para los mensajes del GUI

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//				//////////////////////////////////////////////////////////////////////////////////////////////////////////////						//
	//				//						sidereal-rotation-synchronous high-precision astro-clock							//
	//				//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// with this method we just need an object whose sidereal rotation period is exactly 24 hours so we can
	// track its rotation angle and determine the current time of the day according to it. As such an object
	// doesn't exist in our solar system we had to make up one (we named it "Astro Clock"). We use this
	// method so the acceleration timing works as accurately as possible
	double period_in_seconds = 86400.0;

	double rot_angle_now = angle_from_two_vectors(vector_rotation_by_quaternion({ 1, 0, 0 }, object[1].getTotalRotation()), { 1, 0, 0 });
	double conv_angle = 0.0;

	/////////////////////////////////////////////////////////
	// calculos para convertir de angulos 180 a angulos 360
	/////////////////////////////////////////////////////////
	if (rot_angle_now < rot_angle_before)
	{
		conv_angle = (2.0 * phi_d) - rot_angle_now;
	}
	else conv_angle = rot_angle_now;

	if (conv_angle < conv_angle_before)
	{
		calculations_epoch++; // anhadimos un dia si el angulo actual es menor al anterior
	}

	conv_angle_before = conv_angle;
	conv_angle = (conv_angle / (2.0 * phi_d));

	clock_rotation_time = (conv_angle*period_in_seconds);

	rot_angle_before = rot_angle_now;
	full_date = calculations_epoch + (clock_rotation_time / (object[1].getSiderealRotationPeriod() * 60 * 60));
	if (sync_) true_julian = 2440587.5 + (time(NULL) / 86400.0);
	//																																					//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//											Checkear si la camara esta dentro o fuera de la atmosfera												//
	// un valor <=1 es que la camara esta dentro de la atmosfera, 1 es la altura maxima de la atmosfera, 0 es el nivel del mar
	// (length(cameraPos)-planet_scale)/(atmosphere_scale-planet_scale)

	float orbited_scale_div_two = object[object[control].getOrbiting()].getScale_b() / 2.0f;

	if (object[object[control].getOrbiting()].getHasAtmosphere())
	{
		camera_in_atmosphere = (distances_f(camera->GetPosition(), object[object[control].getOrbiting()].getPosition_b()) -
			orbited_scale_div_two) /
			((object[object[control].getOrbiting()].getScale_b()*object[object[control].getOrbiting()].getAtmosphereRatio() / 2.0f) -
			orbited_scale_div_two);

		camera_altitude = camera_in_atmosphere;
	}

	else
	{
		camera_in_atmosphere = 2.f;
		camera_altitude = (distances_f(camera->GetPosition(), object[object[control].getOrbiting()].getPosition_b()) -
			orbited_scale_div_two) /
			((object[object[control].getOrbiting()].getScale_b()*0.505f) -
			orbited_scale_div_two);
	}
	//																																					//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//											Checkear si la camara esta sobre o bajo las nubes														//
	if (object[object[control].getOrbiting()].getHasClouds() && object[control].getType() == 4)
	{
		under_clouds = (distances_f(camera->GetPosition(), object[object[control].getOrbiting()].getPosition_b()) -
			(object[object[control].getOrbiting()].getScale_b() / 2.0f)) /
			((object[object[control].getOrbiting()].getScale_b()*object[object[control].getOrbiting()].getCloudsRatio() / 2.0f) -
			object[object[control].getOrbiting()].getScale_b() / 2.0f);
	}

	else under_clouds = 2.f; // si el objeto no es un vehiculo entonces las nubes se muestran siempre de frente y no del reves
	//																																					//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//GUI->set_angulos_(angle_from_two_vectors(d3d_to_vec(object[control].getPosition_b() - object[object[control].getOrbiting()].getPosition_b()), auto_->GetAxisY(object[control].getTotalRotation())));

	//*
	// si el objeto orbitado es una esfera, la convertimos en una esfera dinamica cuando la camara alcanza la altitud definida.
	// reduce la velocidad del programa y las fps pero aumenta la precision de los graficos en el contacto.
	int obj_ = object[control].getOrbiting();
	objectClass& objeto = object[obj_];

	if ((objeto.getModel() >= 0 && objeto.getModel() <= 6) || objeto.getModel() == 8)
	{
		if (camera_altitude <= landing_pad_view_altitude)
		{
			objeto.setModel(8);
			update_dynamic_sphere(max(0, int(camera_altitude * 7)), min(11, max(0, int((1 - camera_altitude) * 12))));
			object[3].setVisible(true);
		}

		else
		{
			object[3].setVisible(false);
			objeto.setModel(0);
		}
	}

	return true;
}

void showMasterClass::toggle_sync(bool status)
{
	sync_ = status;

	max_out = status;

	if (!max_out)
	{
		time_acceleration = 1;
		toggle_engine(false);
	}

	else
	{
		toggle_engine(true);
	}
}

bool showMasterClass::RenderSceneToTexture()
{
	bool has_transparencies = false;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget();

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	light_->GenerateViewMatrix();

	for (int n = 0; n < total_objects; n++)
	{
		if (n != 1) // exclude the astro-clock
		{
			if (object[n].getVisible() && object[n].getCastsShadows() && n != 0)
			{
				DirectX->setWorldMatrix(object[n].getWorld());
				set_special_objects_properties(n);
				set_near_and_far_points(n);	// render huge distant objects correctly

				model[object[n].getModel()].set_buffers();

				// dibujar sombras con transparencias
				if (model[object[control].getModel()].get_HasTransparencies() || object[control].getHasRings() ||
					object[object[control].getOrbiting()].getHasRings())
				{
					DirectX->enable_transparencies();
					m_DepthShader_tr->set_transparent_depth_shader(model[object[n].getModel()].GetIndexCount());

					for (int material_ = 0; material_ < model[object[n].getModel()].GetMaterialCount(); material_++)
					{
						if (model[object[n].getModel()].getTransparency(material_) == 1)
						{
							set_texture_array(n, material_); // set the textures to be sent to the shader

							if (!m_DepthShader_tr->SetShaderParameters(DirectX->GetWorldMatrix(), light_->GetViewMatrix(), light_->GetOrthoMatrix(),
								texture_array[0], model[object[n].getModel()].getTransparency(material_))) return false;

							if (object[n].getModel() == 8 || object[n].getModel() == 9) DirectX->GetDeviceContext()->Draw(model[object[n].getModel()].GetVertexCount(), 0);
							else
							{
								if (model[object[n].getModel()].GetMaterialName(material_) == "landing_gear")
								{
									if (object[n].getLandingGear()) DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(material_),
										model[object[n].getModel()].GetIndexStart(material_), 0);
								}

								else if (model[object[n].getModel()].GetMaterialName(material_) == "Windshield" && in_cockpit)
								{
									// do not draw the windshield from the inside
								}

								else DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(material_),
									model[object[n].getModel()].GetIndexStart(material_), 0);
							}
						}

						else has_transparencies = true;
					}

					// No se como hacer para que los materiales con transparencias no se sobrepongan a los opacos que estan detras
					// cuando se dibujan las sombras, mientras tanto voy a dejar de dibujar las sombras de los materiales transparentes.
					// Lo ideal seria dibujar las sombras de acuerdo a sus transparencias pero sin dejar de dibujar las sombras
					// de los materiales opacos que estan detras.
					/*
					if (has_transparencies)
					{
					for (int material_ = 0; material_ < model[object[n].getModel()].GetMaterialCount(); material_++)
					{
					if (model[object[n].getModel()].getTransparency(material_) < 1)
					{
					set_texture_array(n, material_); // set the textures to be sent to the shader

					if (!m_DepthShader_tr->SetShaderParameters(DirectX->GetDeviceContext(),
					DirectX->GetWorldMatrix(), light_->GetViewMatrix(), light_->GetOrthoMatrix(), texture_array[0],
					model[object[n].getModel()].getTransparency(material_))) return false;

					DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(material_),
					model[object[n].getModel()].GetIndexStart(material_), 0);
					}
					}
					}
					*/
				}

				// dibujar sombras sin transparencias
				else
				{
					m_DepthShader->set_depth_shader(model[object[n].getModel()].GetIndexCount());

					if (!m_DepthShader->SetShaderParameters(DirectX->GetWorldMatrix(), light_->GetViewMatrix(), light_->GetOrthoMatrix())) return false;

					if (object[n].getModel() == 8 || object[n].getModel() == 9) DirectX->GetDeviceContext()->Draw(model[object[n].getModel()].GetVertexCount(), 0);
					else
					{
						for (int nn = 0; nn < model[object[n].getModel()].GetMaterialCount(); nn++)
						{
							if (model[object[n].getModel()].GetMaterialName(nn) == "landing_gear")
							{
								if (object[n].getLandingGear()) DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(nn),
									model[object[n].getModel()].GetIndexStart(nn), 0);
							}

							else if (model[object[n].getModel()].GetMaterialName(nn) == "Windshield" && in_cockpit)
							{
								// do not draw the windshield from the inside
							}

							else DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(nn),
								model[object[n].getModel()].GetIndexStart(nn), 0);
						}
					}
				}
			}
		}
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	DirectX->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	DirectX->ResetViewport();

	DirectX->disable_transparencies();

	return true;
}

bool showMasterClass::Update_all_objects()
{
	static float mass_manager_time_sec = 0.f;
	bool mass_manager_performed = false;
	mass_manager_time_sec += dt_b;

	for (int n = 0; n < total_objects; n++)
	{
		// mass manager stuff
		// *** DEBUG *** solo actualiza las masas de todos los objetos si el objeto control no esta en bullet
		if (!object[control].getInBullet())
		{
			if (!mass_manager_performed &&
				object[n].getInAtmosphere() &&
				mass_manager_time_sec >= 1.f &&
				object[n].getMECurrentPower()>0.0 &&
				object[n].getIsParent()
				)
			{
				mass_manager_performed = true;
				mass_manager_time_sec = 0.f;
				script_->mass_manager();
			}
		}
		//////////////////////////////////////////////////////////////////////////

		// actualizamos las animaciones
		if (object[n].getType() == 4) animation->PerformAnimations(n);

		// Establecemos la visibilidad de los objetos
		if (n != 1) // exclude the astro-clock and destroyed objects
		{
			object[n].setDistanceToCamera(distances_f(object[n].getPosition_b(), camera->GetPosition()));
			if (inside_range(n, 2000.0f))
			{
				if (n != 3) object[n].setVisible(true); // si el objeto no es el landing pad, los demas objetos dentro del rango especificado son visibles.
				if (!object[n].getLoaded_tm())
				{
					// cargamos las texturas y modelos de los objetos visibles solamente, si es que las mismas ya no fueron cargadas antes
					if (!update_model(n)) return false;
					if (!update_texture(n)) return false;
					object[n].setLoaded_tm(true);
					ready_to_render = false; // if any object is not loaded we will halt the rendering until every object is good and ready
				}
			}

			else object[n].setVisible(false);

			if (object[n].getType() == 6)
			{
				if (camera_in_atmosphere <= 1) object[n].setVisible(false);
				else object[n].setVisible(true);
			}
		}

		if (object[n].getVisible())
		{
			// transformamos la World Matrix de todos los objetos visibles
			transform_object(n); // rotate, scale and translate the object

			// establecemos si el objeto esta iluminado por el sol o no
			if (object[n].getType() == 3 || object[n].getType() == 4 || object[n].getType() == 10 || object[n].getType() == 11)
			{
				if (sunlit(normalized(d3d_to_vec(object[n].getPosition_b()) - d3d_to_vec(light_->GetPosition())),
					d3d_to_vec(object[n].getPosition_b()),
					d3d_to_vec(object[object[n].getOrbiting()].getPosition_b()),
					object[object[n].getOrbiting()].getScale_b() / 2.0f)) object[n].setSunlit(true);

				else object[n].setSunlit(false);
			}
		}

		if (inside_range(n, bullet_range))
		{
			// Check if an object is not in bullet and it should
			if (object[n].getType() == 4 && !object[n].getInBullet() && object[n].getCollisionShapeType() != -1 && Config.system_initialized) bullet_reload = true;
		}

		// Check if an object is in bullet and it shouldn't
		else if (object[n].getType() == 4 && object[n].getInBullet()) bullet_reload = true;


		// *** DEBUG *** un intento de arreglar el error de alineacion del RCS cuando cambiamos la vista a un objeto muy distante,
		// pero no funciona muy bien
		if (n != control)
		{
			if (object[n].getPro_grade() || object[n].getRetro_grade() || object[n].getNormal() || object[n].getAnti_normal())
			{
				object[n].setOrbitalPosition(object[n].getPosition() - object[object[n].getOrbiting()].getPosition());
				object[n].setOrbitalVelocity(object[n].getVelocity() - object[object[n].getOrbiting()].getVelocity());
			}
		}

		if (object[n].getModel() == 8 && object[control].getOrbiting() != n) object[n].setModel(0); // reseteamos el modelo esferico estatico si no estamos orbitando el objeto
	}

	return true;
}

void showMasterClass::fly_by_camera(Vector3 lookat_pos_)
{
	lookat_pos_.Normalize();
	lookat_pos_ *= -3;

	Vector3 axis_ = vec_to_d3d(GetAxisX(object[control].getTotalRotation()));
	lookat_pos_ += axis_*-1.2f;

	camera->setPosition(lookat_pos_);
}

void showMasterClass::shadows_manager()
{
	// si el objeto orbitado tiene anillos entonces transferir todos los calculos de sombreado a dicho objeto y sus anillos
	// para una visualizacion mas realista, el objeto controlado pierde entonces su sombreado
	if (object[object[control].getOrbiting()].getHasRings())
	{
		object[object[control].getOrbiting()].setCastsShadows(true);
		if (control != star_object) object[object[object[control].getOrbiting()].getRingObject()].setCastsShadows(true);
		object[2].setCastsShadows(true);

		SHADOWMAP_DEPTH = object[object[control].getOrbiting()].getScale_b() * 100.0f;
		SHADOWMAP_NEAR = object[object[control].getOrbiting()].getScale_b() / 100.0f;

		light_->SetLookAt(object[object[control].getOrbiting()].getPosition_b());

		object[2].setPosition_b(object[object[control].getOrbiting()].getPosition_b());
		object[2].setScale_b(SHADOWMAP_DEPTH*1.2f);

		float valor_a = 5 * object[object[control].getOrbiting()].getScale_b();
		float valor_b = 80 * object[object[control].getOrbiting()].getScale_b();

		m_RenderTexture->set_matrices(SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

		light_->GenerateOrthoMatrix(valor_a, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
		light_->setMultiply(valor_b);

		switched_shadows = true;
	}

	else
	{
		if (switched_shadows)
		{
			// set all objects to not cast shadows except for ground objects and landed objects
			for (int n = 0; n < total_objects; n++)
			{
				if (object[n].getType() != 10 && !object[n].getLanded()) object[n].setCastsShadows(false);
			}

			object[control].setCastsShadows(true);
			if (object[control].getHasRings()) object[object[control].getRingObject()].setCastsShadows(true);
			object[2].setCastsShadows(true);

			SHADOWMAP_DEPTH = 100.0f;
			SHADOWMAP_NEAR = 0.1f;

			light_->SetLookAt({ 0.0f, 0.0f, 0.0f });

			object[2].setPosition_b({ 0.0f, 0.0f, 0.0f });
			object[2].setScale_b(SHADOWMAP_DEPTH*1.2f);

			m_RenderTexture->set_matrices(SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
			light_->GenerateOrthoMatrix(light_view_position_to_cast_shadows, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

			light_->setMultiply(light_view_position_to_cast_shadows);

			switched_shadows = false;
		}
	}
}

bool showMasterClass::render_3D_lines()
{
	set_near_and_far_points(control); // para dibujar las lineas 3D correctamente en relacion al objeto controlado
	//DirectX->setNearFar(0.2f, 1e5f);
	Color color_;

	if (Config.draw_orbits)
	{
		for (int n = 0; n < total_objects; n++)
		{
			if (object[n].getOrbiting() == control)
			{
				if (object[n].getType() != 5 && object[n].getType() != 6 && object[n].getType() != 7 && object[n].getType() != 8)
				{
					if (object[n].getType() == 3) color_ = color->Azure04;
					else if (object[n].getType() == 4) color_ = color->Banana;
					else if (object[n].getType() == 1) color_ = color->Orange;
					else color_ = color->Orange;
					color_.A(0.5f);
					if (!draw_3D_orbit(n, color_)) return false;
				}
			}
		}
	}

	if (Config.draw_constellations)
	{
		color_ = color->Light_blue04;
		if (!draw_constellations(color->dim_color(0.25f, color_))) return false;
	}

	//if (!draw_loose_line(object[control].getDockingPortWorldPosition(0), object[control].getDockingPortWorldOrientation(0), color->Yellow)) return false;
	//if (!draw_loose_line(object[control].getPosition_b(), vec_to_d3d(aero_dynamics->getTrueVelocity())*universe_scale, color->Yellow)) return false;
	//if (!draw_loose_line(object[control].getPosition_b(), vec_to_d3d(aero_dynamics->getDynamicPressure())*universe_scale, color->Red)) return false;

	//vec3 vDir = vector_vector_rotation_by_angle(GetAxisZ(object[control].getTotalRotation()), GetAxisY(object[control].getTotalRotation()), object[control].getGimbalAngle());
	/*
	Vector3 axis_X = vec_to_d3d(GetAxisX(object[control].getTotalRotation()));
	Vector3 axis_Y = vec_to_d3d(GetAxisY(object[control].getTotalRotation()));
	Vector3 axis_Z = vec_to_d3d(GetAxisZ(object[control].getTotalRotation()));
	
	Vector3 h_level = object[control].getPosition_b() - object[object[control].getOrbiting()].getPosition_b();
	Vector3 pro_grade = vec_to_d3d(object[control].getOrbitalVelocity());

	draw_loose_line(pro_grade, { 0.f, 0.f, 0.f }, color->White);
	
	draw_loose_line(axis_X, -axis_X, color->Red);
	draw_loose_line(axis_Y, -axis_Y, color->Green);
	draw_loose_line(axis_Z, -axis_Z, color->Blue);
	*/
	//draw_loose_line(object[control].getPosition_b(), Vector3{ 0, 1, 0 }, color->Yellow);

	DirectX->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	return true;
}

bool showMasterClass::draw_3D_orbit(int objeto, Color color_)
{
	if (object[objeto].getDockedTo() == -1) // si el objeto no esta acoplado entonces dibujar su orbita
	{
		//MessageBoxA(m_hwnd, object[objeto].getName().c_str(), "Buffer size", MB_OK);

		int buffer_counter = -1;
		int buffer_size = 0;

		Vector3* buffer_;
		//if (object[objeto].getEccentricity() > 0.5) buffer_size = 127;
		//else 
		buffer_size = 46;

		buffer_ = new Vector3[buffer_size];

		GUI->UpdateCartesianToKeplerianElements_inconditional(objeto);

		float trazos = 0.0f;
		bool end_loop = false;
		const float tolerance_ = std::numeric_limits< float >::epsilon();
		float angulos = 0.0f;
		Vector2 positionPerifocal_;
		Vector2 velocityPerifocal_;
		Vector3 puente = { 0.0f, 0.0f, 0.0f };
		float centralBodyGravitationalParameter = float(GRAVITATIONAL_CONSTANT*object[object[objeto].getOrbiting()].getMass());

		while (!end_loop)
		{
			// Pre-compute sines and cosines of involved angles for efficient computation.
			float cosineOfInclination_ = cosf(float(object[objeto].getInclination()));
			float sineOfInclination_ = sinf(float(object[objeto].getInclination()));
			float cosineOfArgumentOfPeriapsis_ = cosf(float(object[objeto].getArgumentOfPerigee()));
			float sineOfArgumentOfPeriapsis_ = sinf(float(object[objeto].getArgumentOfPerigee()));
			float cosineOfLongitudeOfAscendingNode_ = cosf(float(object[objeto].getLongitudeOfAscendingNode()));
			float sineOfLongitudeOfAscendingNode_ = sinf(float(object[objeto].getLongitudeOfAscendingNode()));
			float cosineOfTrueAnomaly_ = cosf(angulos + float(object[objeto].getTrueAnomaly()));
			float sineOfTrueAnomaly_ = sinf(angulos + float(object[objeto].getTrueAnomaly()));

			// Declare semi-latus rectum.
			float semiLatusRectum_ = -0.0f;

			// Compute semi-latus rectum in the case it is not a parabola.
			if (fabsf(float(object[objeto].getEccentricity()) - 1.0f) > tolerance_)
			{
				semiLatusRectum_ = float(object[objeto].getSemiMayorAxis() * (1.0f - (object[objeto].getEccentricity() * object[objeto].getEccentricity())));
			}

			// Else set the semi-latus rectum given for a parabola as the first element in the vector
			// of Keplerian elements..
			else  { semiLatusRectum_ = float(object[objeto].getSemiMayorAxis()); }

			// Definition of position in the perifocal coordinate system.
			positionPerifocal_.x = semiLatusRectum_ * cosineOfTrueAnomaly_
				/ (1.0f + float(object[objeto].getEccentricity()) * cosineOfTrueAnomaly_);
			positionPerifocal_.y = semiLatusRectum_ * sineOfTrueAnomaly_
				/ (1.0f + float(object[objeto].getEccentricity()) * cosineOfTrueAnomaly_);

			// Definition of velocity in the perifocal coordinate system.
			velocityPerifocal_ = Vector2{
				-sqrtf(centralBodyGravitationalParameter / semiLatusRectum_) * sineOfTrueAnomaly_,
				sqrtf(centralBodyGravitationalParameter / semiLatusRectum_)
				* (float(object[objeto].getEccentricity()) + cosineOfTrueAnomaly_) };

			// Definition of the transformation matrix.
			float transformationMatrix_[3][2];

			// Compute the transformation matrix.
			transformationMatrix_[0][0] = cosineOfLongitudeOfAscendingNode_* cosineOfArgumentOfPeriapsis_ - sineOfLongitudeOfAscendingNode_* sineOfArgumentOfPeriapsis_ * cosineOfInclination_;
			transformationMatrix_[0][1] = -cosineOfLongitudeOfAscendingNode_* sineOfArgumentOfPeriapsis_ - sineOfLongitudeOfAscendingNode_* cosineOfArgumentOfPeriapsis_ * cosineOfInclination_;
			transformationMatrix_[1][0] = sineOfLongitudeOfAscendingNode_* cosineOfArgumentOfPeriapsis_ + cosineOfLongitudeOfAscendingNode_* sineOfArgumentOfPeriapsis_ * cosineOfInclination_;
			transformationMatrix_[1][1] = -sineOfLongitudeOfAscendingNode_* sineOfArgumentOfPeriapsis_ + cosineOfLongitudeOfAscendingNode_* cosineOfArgumentOfPeriapsis_ * cosineOfInclination_;
			transformationMatrix_[2][0] = sineOfArgumentOfPeriapsis_ * sineOfInclination_;
			transformationMatrix_[2][1] = cosineOfArgumentOfPeriapsis_ * sineOfInclination_;

			// Compute value of position in Cartesian coordinates.
			puente = object[object[objeto].getOrbiting()].getPosition_b() + (matrix32_Vector2_multiplication(transformationMatrix_, positionPerifocal_)* universe_scale);

			trazos = 0.14f;// int(360 / (1 / dt_a)); // limitamos la cantidad de trazos de orbita de acuerdo a las fps
			//if (object[objeto].getEccentricity() > 0.5 && trazos > 0.05f) trazos = 0.05f; // las orbitas de alta eccentricidad no se dibujan bien con pocos trazos
			if (angulos >= DirectX::XM_2PI) end_loop = true;
			angulos += trazos;
			if (angulos > DirectX::XM_2PI) angulos = DirectX::XM_2PI;

			buffer_counter++;

			//MessageBoxA(m_hwnd, std::to_string(buffer_counter).c_str(), object[objeto].getName().c_str(), MB_OK);

			buffer_[buffer_counter] = puente;
		}

		if (color_.A() < 1.0f) DirectX->enable_transparencies();

		if (!GUI->draw_line_strip(buffer_, buffer_size, color_)) return false;

		SAFE_DELETE_ARRAY(buffer_);
	}

	return true;
}

bool showMasterClass::render_projected_text()
{
	// Creamos los valores a ser usados
	Vector3* position_buffer;
	std::string* text_buffer;
	Color* color_buffer;
	int buffer_size = 0;
	int buffer_counter = -1;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//												Contamos la cantidad de textos a dibujar													//
	if (Config.draw_orbits)
	{
		for (int n = 0; n < total_objects; n++)
		{
			if (object[n].getDockedTo() == -1 && object[n].getOrbiting() == control)
			{
				if (object[n].getType() == 2 || object[n].getType() == 3 || object[n].getType() == 4)
				{
					buffer_size++;
				}
			}
		}
	}

	if (Config.draw_stars) buffer_size += stars->get_total_stars();
	//																																			//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// creamos los buffers
	position_buffer = new Vector3[buffer_size];
	text_buffer = new std::string[buffer_size];
	color_buffer = new Color[buffer_size];

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														Llenamos los buffers																//
	if (Config.draw_orbits)
	{
		for (int n = 0; n < total_objects; n++)
		{
			if (object[n].getDockedTo() == -1 && object[n].getOrbiting() == control)
			{
				if (object[n].getType() == 2 || object[n].getType() == 3 || object[n].getType() == 4)
				{
					buffer_counter++;
					if (object[n].getType() == 3) color_buffer[buffer_counter] = color->White;
					else if (object[n].getType() == 4) color_buffer[buffer_counter] = color->Banana;
					else if (object[n].getType() == 1) color_buffer[buffer_counter] = color->Orange;
					else color_buffer[buffer_counter] = color->Orange;
					color_buffer[buffer_counter].A(0.5f);

					Vector3 pos_ = object[n].getPosition_b();

					if (n == control) position_buffer[buffer_counter] = pos_ + Vector3{ 0.01f, 0.01f, 0.01f };
					else position_buffer[buffer_counter] = pos_;
					text_buffer[buffer_counter] = " < " + object[n].getName();
				}
			}
		}
	}

	if (Config.draw_stars)
	{
		for (int n = 0; n < stars->get_total_stars(); n++)
		{
			buffer_counter++;
			Color color_ = color->Azure04;
			color_.A(0.4f);
			color_buffer[buffer_counter] = color_;
			text_buffer[buffer_counter] = " < " + stars->get_star_data(n).name;
			position_buffer[buffer_counter] = stars->get_star_data(n).position_b + camera->GetPosition();
		}
	}
	//																																			//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// enviamos el paquete completo para su dibujo en pantalla
	GUI->draw_projected_text(position_buffer, text_buffer, color_buffer, buffer_size);

	// eliminamos los pointers
	SAFE_DELETE_ARRAY(position_buffer);
	SAFE_DELETE_ARRAY(text_buffer);
	SAFE_DELETE_ARRAY(color_buffer);

	return true;
}

bool showMasterClass::draw_constellations(Color color_)
{
	int total_constellations = stars->get_total_constellations();

	if (total_constellations > 0)
	{
		for (int n = 0; n < total_constellations; n++)
		{
			int buffer_size = stars->get_constellation_data(n).lines_in_constellation;
			Vector3* buffer_ = new Vector3[buffer_size];

			for (int nn = 0; nn < buffer_size; nn++)
			{
				buffer_[nn] = stars->get_constellation_data(n).points_positions[nn] + camera->GetPosition();
			}

			if (color_.A() < 1.0f) DirectX->enable_transparencies();

			if (!GUI->draw_line_strip(buffer_, buffer_size, color_)) return false;

			SAFE_DELETE_ARRAY(buffer_);
		}
	}

	return true;
}

bool showMasterClass::draw_loose_line(Vector3 origin_, Vector3 target_, Color color_)
{
	int buffer_size = 2;
	Vector3* buffer_ = new Vector3[buffer_size];

	buffer_[0] = origin_;
	buffer_[1] = target_;

	if (!GUI->draw_line_strip(buffer_, buffer_size, color_)) return false;

	SAFE_DELETE_ARRAY(buffer_);

	return true;
}

bool showMasterClass::update_dynamic_sphere(int main_iterations, int sub_iterations)
{
	// valores para checkear si se cambio la cantidad de poligonos del modelo
	static int main_iter = main_iterations;
	static int sub_iter = sub_iterations;

	int total_facets = int(powf(4.0f, float(main_iterations)) * 8.0f) + (38 * sub_iterations) + sub_iterations;

	// Check para reconstruir el modelo solo si el hitface cambia
	Vector3 origen = object[control].getPosition_b();
	Vector3 direccion = object[object[control].getOrbiting()].getPosition_b() - origen;

	// Create the local transformation matrix
	int cuerpo = object[control].getOrbiting();
	Matrix escalar, rotar, trasladar, local_matrix;
	float esc_ = object[cuerpo].getScale_b() / 2.0f;
	if (object[cuerpo].getPolarRadius() < object[cuerpo].getEquatorialRadius()) escalar = DirectX::XMMatrixScaling(esc_, esc_* (1.0f - object[cuerpo].getFlattening()), esc_);
	else if (object[cuerpo].getPolarRadius() < object[cuerpo].getEquatorialRadius()) escalar = DirectX::XMMatrixScaling(esc_* (1.0f - object[cuerpo].getFlattening()), esc_, esc_);
	else escalar = DirectX::XMMatrixScaling(esc_, esc_, esc_);
	rotar = DirectX::XMMatrixRotationQuaternion(quat_to_d3dquat(object[cuerpo].getTotalRotation()));
	Vector3 tras_ = object[cuerpo].getPosition_b();
	trasladar = DirectX::XMMatrixTranslation(tras_.x, tras_.y, tras_.z);
	local_matrix = escalar*rotar*trasladar;

	if (!rayIntersectsTriangle(origen, direccion, RowVector3byMatrix(hit_single_face.p1, local_matrix), RowVector3byMatrix(hit_single_face.p2, local_matrix), RowVector3byMatrix(hit_single_face.p3, local_matrix))
		|| main_iter != main_iterations || sub_iter != sub_iterations)
	{
		FACET* facet = new FACET[total_facets];

		Vector3 p[6] = { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, -1.0f, 0.0f },
		{ 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f } }; // the intial shape, a double pyramid

		/* Create the level 0 object */
		float a = 1 / sqrtf(2.0);
		for (int i = 0; i < 6; i++)
		{
			p[i].x *= a;
			p[i].y *= a;
		}

		facet[0].p1 = p[0]; facet[0].p2 = p[3]; facet[0].p3 = p[4];
		facet[1].p1 = p[0]; facet[1].p2 = p[4]; facet[1].p3 = p[5];
		facet[2].p1 = p[0]; facet[2].p2 = p[5]; facet[2].p3 = p[2];
		facet[3].p1 = p[0]; facet[3].p2 = p[2]; facet[3].p3 = p[3];
		facet[4].p1 = p[1]; facet[4].p2 = p[4]; facet[4].p3 = p[3];
		facet[5].p1 = p[1]; facet[5].p2 = p[5]; facet[5].p3 = p[4];
		facet[6].p1 = p[1]; facet[6].p2 = p[2]; facet[6].p3 = p[5];
		facet[7].p1 = p[1]; facet[7].p2 = p[3]; facet[7].p3 = p[2];
		int facet_number = 8;
		int former_facet_number = 0;
		Vector3 pa, pb, pc;

		// seccionar el modelo el numero de iteraciones principales
		if (main_iterations > 0)
		{
			/* Bisect each edge and move to the surface of a unit sphere */
			for (int it = 0; it < main_iterations; it++) {
				former_facet_number = facet_number;
				for (int i = 0; i < former_facet_number; i++)
				{
					pa.x = (facet[i].p1.x + facet[i].p2.x) / 2;
					pa.y = (facet[i].p1.y + facet[i].p2.y) / 2;
					pa.z = (facet[i].p1.z + facet[i].p2.z) / 2;
					pb.x = (facet[i].p2.x + facet[i].p3.x) / 2;
					pb.y = (facet[i].p2.y + facet[i].p3.y) / 2;
					pb.z = (facet[i].p2.z + facet[i].p3.z) / 2;
					pc.x = (facet[i].p3.x + facet[i].p1.x) / 2;
					pc.y = (facet[i].p3.y + facet[i].p1.y) / 2;
					pc.z = (facet[i].p3.z + facet[i].p1.z) / 2;

					pa.Normalize();
					pb.Normalize();
					pc.Normalize();

					facet[facet_number].p1 = facet[i].p1; facet[facet_number].p2 = pa; facet[facet_number].p3 = pc; facet_number++;
					facet[facet_number].p1 = pa; facet[facet_number].p2 = facet[i].p2; facet[facet_number].p3 = pb; facet_number++;
					facet[facet_number].p1 = pb; facet[facet_number].p2 = facet[i].p3; facet[facet_number].p3 = pc; facet_number++;

					facet[i].p1 = pa;
					facet[i].p2 = pb;
					facet[i].p3 = pc;
				}
			}
		}

		int new_facet_number = facet_number;
		int hasta_facet_number = facet_number;

		hit_face = hit_facet(object[control].getOrbiting(), object[control].getPosition_b(), object[object[control].getOrbiting()].getPosition_b(), total_facets, facet);

		for (int uu = 0; uu < sub_iterations; uu++) // numero de niveles de division de los triangulos elegidos
		{
			for (int i = 0; i < new_facet_number; i++)
			{
				if (i == hit_face)
				{
					for (int ext = 0; ext < new_facet_number; ext++)
					{
						if (i != ext)
						{
							if ((facet[i].p1.x == facet[ext].p1.x && facet[i].p1.y == facet[ext].p1.y && facet[i].p1.z == facet[ext].p1.z) ||
								(facet[i].p2.x == facet[ext].p1.x && facet[i].p2.y == facet[ext].p1.y && facet[i].p2.z == facet[ext].p1.z) ||
								(facet[i].p3.x == facet[ext].p1.x && facet[i].p3.y == facet[ext].p1.y && facet[i].p3.z == facet[ext].p1.z) ||

								(facet[i].p1.x == facet[ext].p2.x && facet[i].p1.y == facet[ext].p2.y && facet[i].p1.z == facet[ext].p2.z) ||
								(facet[i].p2.x == facet[ext].p2.x && facet[i].p2.y == facet[ext].p2.y && facet[i].p2.z == facet[ext].p2.z) ||
								(facet[i].p3.x == facet[ext].p2.x && facet[i].p3.y == facet[ext].p2.y && facet[i].p3.z == facet[ext].p2.z) ||

								(facet[i].p1.x == facet[ext].p3.x && facet[i].p1.y == facet[ext].p3.y && facet[i].p1.z == facet[ext].p3.z) ||
								(facet[i].p2.x == facet[ext].p3.x && facet[i].p2.y == facet[ext].p3.y && facet[i].p2.z == facet[ext].p3.z) ||
								(facet[i].p3.x == facet[ext].p3.x && facet[i].p3.y == facet[ext].p3.y && facet[i].p3.z == facet[ext].p3.z))
							{
								pa.x = (facet[ext].p1.x + facet[ext].p2.x) / 2;
								pa.y = (facet[ext].p1.y + facet[ext].p2.y) / 2;
								pa.z = (facet[ext].p1.z + facet[ext].p2.z) / 2;
								pb.x = (facet[ext].p2.x + facet[ext].p3.x) / 2;
								pb.y = (facet[ext].p2.y + facet[ext].p3.y) / 2;
								pb.z = (facet[ext].p2.z + facet[ext].p3.z) / 2;
								pc.x = (facet[ext].p3.x + facet[ext].p1.x) / 2;
								pc.y = (facet[ext].p3.y + facet[ext].p1.y) / 2;
								pc.z = (facet[ext].p3.z + facet[ext].p1.z) / 2;

								pa.Normalize();
								pb.Normalize();
								pc.Normalize();

								facet[facet_number].p1 = facet[ext].p1; facet[facet_number].p2 = pa; facet[facet_number].p3 = pc; facet_number++;
								facet[facet_number].p1 = pa; facet[facet_number].p2 = facet[ext].p2; facet[facet_number].p3 = pb; facet_number++;
								facet[facet_number].p1 = pb; facet[facet_number].p2 = facet[ext].p3; facet[facet_number].p3 = pc; facet_number++;

								facet[ext].p1 = pa;
								facet[ext].p2 = pb;
								facet[ext].p3 = pc;
							}
						}
					}

					pa.x = (facet[i].p1.x + facet[i].p2.x) / 2;
					pa.y = (facet[i].p1.y + facet[i].p2.y) / 2;
					pa.z = (facet[i].p1.z + facet[i].p2.z) / 2;
					pb.x = (facet[i].p2.x + facet[i].p3.x) / 2;
					pb.y = (facet[i].p2.y + facet[i].p3.y) / 2;
					pb.z = (facet[i].p2.z + facet[i].p3.z) / 2;
					pc.x = (facet[i].p3.x + facet[i].p1.x) / 2;
					pc.y = (facet[i].p3.y + facet[i].p1.y) / 2;
					pc.z = (facet[i].p3.z + facet[i].p1.z) / 2;

					pa.Normalize();
					pb.Normalize();
					pc.Normalize();

					facet[facet_number].p1 = facet[i].p1; facet[facet_number].p2 = pa; facet[facet_number].p3 = pc; if (facet_number + 1 < total_facets) facet_number++;
					facet[facet_number].p1 = pa; facet[facet_number].p2 = facet[i].p2; facet[facet_number].p3 = pb; if (facet_number + 1 < total_facets) facet_number++;
					facet[facet_number].p1 = pb; facet[facet_number].p2 = facet[i].p3; facet[facet_number].p3 = pc; if (facet_number + 1 < total_facets) facet_number++;

					facet[i].p1 = pa;
					facet[i].p2 = pb;
					facet[i].p3 = pc;
				}
			}

			new_facet_number = facet_number;
			hasta_facet_number = facet_number;
			hit_face = hit_facet(object[control].getOrbiting(), object[control].getPosition_b(), object[object[control].getOrbiting()].getPosition_b(), total_facets, facet);

		}

		/*
		Vector3 pp1 = facet[hit_face].p1;
		Vector3 pp2 = facet[hit_face].p2;
		Vector3 pp3 = facet[hit_face].p3;

		FACET exp_facet = { pp1, pp2, pp3 };

		model[8].update_dynamic_sphere_VertexBuffer(3, &exp_facet);
		*/

		//*
		int count_ = 0;
		for (int n = 0; n < facet_number; n++)
		{
			if (n != hit_face)
			{
				if ((facet[hit_face].p1.x == facet[n].p1.x && facet[hit_face].p1.y == facet[n].p1.y && facet[hit_face].p1.z == facet[n].p1.z) ||
					(facet[hit_face].p2.x == facet[n].p1.x && facet[hit_face].p2.y == facet[n].p1.y && facet[hit_face].p2.z == facet[n].p1.z) ||
					(facet[hit_face].p3.x == facet[n].p1.x && facet[hit_face].p3.y == facet[n].p1.y && facet[hit_face].p3.z == facet[n].p1.z) ||

					(facet[hit_face].p1.x == facet[n].p2.x && facet[hit_face].p1.y == facet[n].p2.y && facet[hit_face].p1.z == facet[n].p2.z) ||
					(facet[hit_face].p2.x == facet[n].p2.x && facet[hit_face].p2.y == facet[n].p2.y && facet[hit_face].p2.z == facet[n].p2.z) ||
					(facet[hit_face].p3.x == facet[n].p2.x && facet[hit_face].p3.y == facet[n].p2.y && facet[hit_face].p3.z == facet[n].p2.z) ||

					(facet[hit_face].p1.x == facet[n].p3.x && facet[hit_face].p1.y == facet[n].p3.y && facet[hit_face].p1.z == facet[n].p3.z) ||
					(facet[hit_face].p2.x == facet[n].p3.x && facet[hit_face].p2.y == facet[n].p3.y && facet[hit_face].p2.z == facet[n].p3.z) ||
					(facet[hit_face].p3.x == facet[n].p3.x && facet[hit_face].p3.y == facet[n].p3.y && facet[hit_face].p3.z == facet[n].p3.z))
				{
					tf[count_] = facet[n];
					facet[n] = { { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f, } };
					count_++;
				}
			}
		}

		tf[12] = facet[hit_face];

		facet[hit_face] = { { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f, }, { 0.0f, 0.0f, 0.0f, } };

		model[8].update_dynamic_sphere_VertexBuffer(total_facets * 3, facet);

		object[3].setTexture1(object[object[control].getOrbiting()].getTexture1());
		object[3].setTexture2(object[object[control].getOrbiting()].getTexture2());
		object[3].setTexture3(object[object[control].getOrbiting()].getTexture3());
		object[3].setHasAtmosphere(object[object[control].getOrbiting()].getHasAtmosphere());
		object[3].setHasNightMap(object[object[control].getOrbiting()].getHasNightMap());
		object[3].setHasSpecularMap(object[object[control].getOrbiting()].getHasSpecularMap());

		SAFE_DELETE_ARRAY(facet);
	}

	main_iter = main_iterations;
	sub_iter = sub_iterations;

	return true;
}

int showMasterClass::hit_facet(int cuerpo, Vector3 origen, Vector3 direccion, int total_facets, FACET* facet)
{
	direccion -= origen;
	FACET ff;

	for (int n = 0; n < total_facets; n++)
	{
		ff = facet[n];

		// Create the local transformation matrix
		Matrix escalar, rotar, trasladar, local_matrix;
		float esc_ = object[cuerpo].getScale_b() / 2.0f;
		if (object[cuerpo].getPolarRadius() < object[cuerpo].getEquatorialRadius()) escalar = DirectX::XMMatrixScaling(esc_, esc_* (1.0f - object[cuerpo].getFlattening()), esc_);
		else if (object[cuerpo].getPolarRadius() < object[cuerpo].getEquatorialRadius()) escalar = DirectX::XMMatrixScaling(esc_* (1.0f - object[cuerpo].getFlattening()), esc_, esc_);
		else escalar = DirectX::XMMatrixScaling(esc_, esc_, esc_);
		rotar = DirectX::XMMatrixRotationQuaternion(quat_to_d3dquat(object[cuerpo].getTotalRotation()));
		Vector3 tras_ = object[cuerpo].getPosition_b();
		trasladar = DirectX::XMMatrixTranslation(tras_.x, tras_.y, tras_.z);
		local_matrix = escalar*rotar*trasladar;

		if (rayIntersectsTriangle(origen, direccion, RowVector3byMatrix(ff.p1, local_matrix), RowVector3byMatrix(ff.p2, local_matrix), RowVector3byMatrix(ff.p3, local_matrix)))
		{
			hit_single_face = ff;
			return n;
		}
	}

	return 0;
}

bool showMasterClass::rayIntersectsTriangle(Vector3 origen, Vector3 direccion, Vector3 v1, Vector3 v2, Vector3 v3)
{
	float small_number = 1e-15f;
	Vector3 e1, e2, h, s, q;
	float a, f, u, v;
	e1 = v2 - v1;
	e2 = v3 - v1;

	h = direccion.Cross(e2);
	a = e1.Dot(h);

	if (a > -small_number && a < small_number)
		return(false);

	f = 1.0f / a;
	s = origen - v1;
	float provi = s.Dot(h);
	u = f * provi;

	if (u < 0.0f || u > 1.0f)
		return(false);

	q = s.Cross(e1);
	provi = direccion.Dot(q);
	v = f * provi;

	if (v < 0.0f || u + v > 1.0f)
		return(false);

	// at this stage we can compute t to find out where
	// the intersection point is on the line
	provi = e2.Dot(q);
	float t = f * provi;

	if (t > small_number)
	{ // ray intersection
		//vector3 intersection_point = vector3_plus_vector3(p, vector3_multiplication_by_double(d, t));
		//distancia_mesh = vector3_distances(intersection_point, p);
		float distancia_mesh = t;
		return true;
	}

	else // this means that there is a line intersection
		// but not a ray intersection
		return (false);
}

bool showMasterClass::camera_manager()
{
	//fly_by_camera(object[9].getPosition_b());
	/*
	Config.display_info = std::to_string(camera->getOrientation().x) + ", " +
		std::to_string(camera->getOrientation().y) + ", " +
		std::to_string(camera->getOrientation().z) + ", " +
		std::to_string(camera->getOrientation().w);
	//*/
	//Config.display_info = std::to_string(camera->getOrientation().x - camera->getOrientation().w);
	

	// Generate the view matrix based on the camera's position.
	if (!camera->update_camera(1.0f / dt_a, input->GetMouseSate(), 0.15f, 0.15f, dt_a,
		quat_to_d3dquat(object[control].getTotalRotation()))) return false;

	if (!in_cockpit)
	{
		if (switched_camera)
		{
			camera->setOrientation({ 0.5f, 0.f, 0.f, 1.f });
			switched_camera = false;
			universe_scale = 1.0f / (float(object[control].getScale()) / 1.5f);
			camera->setInCockpit(false);
		}
		camera->Render({ 0.f, 0.f, 0.f });
	}

	else
	{
		if (switched_camera)
		{
			camera->setInCockpit(true);
			camera->setOrientation({ 0.f, 1.f, 0.f, 0.f });
			switched_camera = false;
			universe_scale = 1.0f / (float(object[control].getScale()) / 2.1f);
		}

		camera->Render_Pos(vec_to_d3d(object[control].getWorldPosition(d3d_to_vec(object[control].getCockpitCameraPosition()))), camera->GetPosition() + vec_to_d3d(-GetAxisZ(object[control].getTotalRotation())));
	}

	// si la camara entra bajo tierra anular la rotacion correspondiente
	/*
	int orbited_ = object[control].getOrbiting();
	vec3 CamVec = d3d_to_vec(camera->GetPosition() - object[orbited_].getPosition_b());

	if (length(CamVec) <= (object[orbited_].getScale_b() / 2.f) && !in_cockpit)
	{
		vec3 CamVec_d = normalized((d3d_to_vec(camera->GetPosition() / universe_scale) + object[control].getPosition()) -
			object[orbited_].getPosition());

		camera->setPositionb(vec_to_d3d(CamVec_d * ((object[orbited_].getScale() / 2.0) + 0.1f*universe_scale) +
			object[orbited_].getPosition() - object[control].getPosition()) * universe_scale);

		camera->setPosition(camera->GetPositionb());

		camera->Renderb({ 0.f, 0.f, 0.f });
	}
	

	// *** DEBUG *** este calculo hace que cuando la camara entrar dentro de la tierra use la posicion del objeto control
	// en vez de la posicion de la camara, lo que hace que los calculos tales como el brillo especular del shader salgan mal
	// pero de momento soluciona el problema de la camara entrando dentro de la tierra
	if (length_f(camera->GetPosition() - object[orbited_].getPosition_b()) <= object[orbited_].getScale_b() / 2.f)
		camera->setPosition(object[control].getPosition_b());
	//*/

	return true;
}

void showMasterClass::landing_manager(int n, float dt_)
{
	int orbiting_ = object[n].getOrbiting();

	vec3 orbited_rotation_vec = aero_dynamics->getOrbitedRotationVec(n); // *** DEBUG *** repeticion

	vec3 vector_ = normalized((object[n].getPosition() + object[n].getVelocity()* dt_) - object[orbiting_].getPosition());
	vec3 new_pos = object[orbiting_].getPosition() + (vector_*((object[orbiting_].getScale() / 2) - object[n].getGroundSensorDistance()));
	object[n].setPosition(new_pos);


	vec3 frente = normalized(GUI->getHorizontalDir(n)); // *** DEBUG *** repeticion
	double new_vel = length(object[n].getOrbitalVelocity() - aero_dynamics->getOrbitedRotationVec(n));

	double angulo = angle_from_two_vectors(object[n].get_true_velocity(), object[n].getPosition() - object[orbiting_].getPosition());
	double ratio = abs(100.0 - (angulo * 100 / DirectX::XM_PIDIV2));
	if (ratio>0.0 && ratio <= 100.0) new_vel -= (ratio * new_vel / 100);

	if (input->KeyPressed(DIK_BACKSPACE)) new_vel *= 0.99;

	// new_vel -= landing_gear_friction; *** TODO ***	

	object[n].setVelocity(frente*(new_vel - length(object[n].get_Drag() / object[n].getMass()*dt_)) + orbited_rotation_vec + object[orbiting_].getVelocity()); // *** DEBUG *** aero_dynamics->getTrueVelocity() solo arroja la velocidad del objeto controlado, los demas objetos van a dar un error de trayectoria
	//																																//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	object[n].setLanded(true);
	object[n].setLandingGear(true);

	if (object[n].getGroundSpeed() < 0.25 && object[n].getMECurrentPower() == 0.0)
	{
		object[n].setGroundSpeed(0.0);
		object[n].setDockedTo(-1);
		object[n].setTopAbsoluteParent(-1);
		object[n].setTopArtificialParent(-1);
		object[n].set_docked_to_name("N/A");
		object[n].setDockReady(true);
		script_->dock_object(n, orbiting_, 20.f);
	}
}

void showMasterClass::crash_manager(int n)
{
	int orbiting_ = object[n].getOrbiting();

	vec3 vector_ = normalized(object[orbiting_].getPosition() - (object[n].getPosition() + object[n].getVelocity()* dt_b));
	object[n].setPosition(object[orbiting_].getPosition() - (vector_*(object[orbiting_].getScale() / 2)));
	object[n].setDockReady(false);
	object[n].setLanded(false);
	script_->undock_object(n);
}

bool showMasterClass::render_manager()
{
	if (!load_starfield()) return false;

	if (!load_planets()) return false;

	if (!render_general_objects()) return false;


	return true;
}

bool showMasterClass::load_starfield()
{
	int n = 0;

	DirectX->disable_transparencies();

	// Set the render target to be the render to texture.
	m_RenderTextureFullScreen->SetRenderTarget();

	// Clear the render to texture.
	m_RenderTextureFullScreen->ClearRenderTarget(0, 0, 0, 1);

	//DirectX->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectX->setWorldMatrix(object[n].getWorld());
	set_special_objects_properties(n);
	set_near_and_far_points(n);	// render huge distant objects correctly

	model[object[n].getModel()].set_buffers();

	// Set the shader
	if (!set_shader_to_use(n)) return false;

	for (int nn = 0; nn < model[object[n].getModel()].GetMaterialCount(); nn++)
	{
		set_texture_array(n, nn); // set the textures to be sent to the shader

		set_shader_parameters(n, nn);

		DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(nn),
			model[object[n].getModel()].GetIndexStart(nn), 0);
	}

	return true;
}

bool showMasterClass::load_planets()
{
	for (int n = 0; n < total_objects; n++)
	{
		if (object[n].getVisible())
		{
			if (((object[n].getType() == 2 || object[n].getType() == 7) && camera_in_atmosphere > 1)
				|| (object[n].getType() == 3 && !inside_range(n, 100.f)) || object[n].getType() == 8)
			{
				DirectX->setWorldMatrix(object[n].getWorld());
				set_special_objects_properties(n);
				set_near_and_far_points(n);	// render huge distant objects correctly

				model[object[n].getModel()].set_buffers();

				// Set the shader
				if (!set_shader_to_use(n)) return false;

				for (int nn = 0; nn < model[object[n].getModel()].GetMaterialCount(); nn++)
				{
					set_texture_array(n, nn); // set the textures to be sent to the shader

					set_shader_parameters(n, nn);

					DirectX->GetDeviceContext()->DrawIndexed(model[object[n].getModel()].GetIndexSize(nn),
						model[object[n].getModel()].GetIndexStart(nn), 0);
				}
			}
		}
	}

	return true;
}

bool showMasterClass::initialize_background_textures()
{
	int f_texture = 0;
	int ft_width = screenWidth;
	int ft_height = screenHeight;
	if (!background_texture->Initialize_from_resource_view(screenWidth, screenHeight,
		texture[0].get_Texture(), ft_width, ft_height)) return false;

	return true;
}

bool showMasterClass::render_background_textures()
{
	DirectX->TurnZBufferOff();
	DirectX->set_inversed_render(false);

	background_texture->Load_dynamic_Texture_from_resource_view(m_RenderTextureFullScreen->GetShaderResourceView());
	if (!background_texture->Render(0, 0)) return false;
	if (!m_TextureShader->Render(background_texture->GetIndexCount(),
		DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), DirectX->GetOrthoMatrix(), background_texture->GetTexture(), 9,
		background_texture->getMouseStatus(), color->White, { 0.f, 0.f, 0.f }, 0.f)) return false;

	DirectX->TurnZBufferOn();

	return true;
}

bool showMasterClass::exclude_background_objects(int n)
{
	if (object[n].getType() == 1) return false;
	else if (object[n].getType() == 2 && camera_in_atmosphere > 1) return false;
	else if (object[n].getType() == 7 && camera_in_atmosphere > 1) return false;
	else if (object[n].getType() == 3 && !inside_range(n, 100.f)) return false;
	else if (object[n].getType() == 8) return false;

	return true;
}

bool showMasterClass::render_engine_particles()
{
	DirectX->set_inversed_render(false);

	shock_diamonds_particles->setcamPos(camera->GetPosition());

	// creamos la matriz de rotacion hacia la camara y la enviamos al instance shader
	Matrix rotar = Matrix_Look_at_Position(object[control].getPosition_b(), camera->GetPosition(), camera->getCamera_yAxis());

	shock_diamonds_particles->update_shock_diamonds(camera_in_atmosphere);

	shock_diamonds_particles->RenderBuffers();

	if (!instanced_shader->Render(shock_diamonds_particles->GetVertexCount(),
		shock_diamonds_particles->GetInstanceCount(),
		rotar, camera->GetViewMatrix(), DirectX->GetProjectionMatrix(),
		shock_diamonds_particles->GetTexture())) return false;

	DirectX->disable_transparencies();

	return true;
}

void showMasterClass::reset_scene()
{
	for (int n = 0; n < total_objects; n++)
	{
		object[n].setVelocityModified(false); // reseteamos el flag de movimiento artificial del objeto;
	}

	Config.changed_scale = false;

	if (switched_object)
	{
		if (object[control].getCameraPreferedAxis() == Vector3{ 0.f, 1.f, 0.f }) camera->setOrientation({ 0.0f, 0.f, 0.f, 1.f });
		else if (object[control].getCameraPreferedAxis() == Vector3{ 0.f, 0.f, -1.f }) camera->setOrientation({ 0.5f, 0.f, 0.f, 0.5f });
		switched_object = false;
	}
}

Mat4d showMasterClass::ground_rotation(int n, double heading_degrees)
{
	int orbited_ = object[n].getOrbiting();

	quat total_rotation = object[orbited_].getTotalRotation();

	quat unrotated = total_rotation / quat{ 0.0, 0.0, 0.0, 1.0 };
	vec3 orbital_position = object[n].getPosition() - object[orbited_].getPosition();
	vec3 rotated_position = vector_rotation_by_quaternion(orbital_position, unrotated);

	quat q_temp = { 0.0, 0.0, 0.0, 1.0 };
	double angle = bullet_physics->getAngulosX(orbited_);
	q_temp.x = 1.0 * sin(angle / 2.0);
	q_temp.w = cos(angle / 2.0);
	total_rotation = q_temp* total_rotation;

	q_temp = { 0.0, 0.0, 0.0, 1.0 };
	angle = bullet_physics->getAngulosY(orbited_);
	q_temp.y = 1.0 * sin(angle / 2.0);
	q_temp.w = cos(angle / 2.0);
	total_rotation = q_temp* total_rotation;

	q_temp = { 0.0, 0.0, 0.0, 1.0 };
	angle = bullet_physics->getAngulosZ(orbited_);
	q_temp.z = 1.0 * sin(angle / 2.0);
	q_temp.w = cos(angle / 2.0);
	total_rotation = q_temp* total_rotation;

	vec3 east = vector_rotation_by_quaternion(rotated_position, ~total_rotation) - orbital_position;
	vec3 west = -east;
	vec3 north = east ^ normalized(orbital_position);
	vec3 south = -north;

	vec3 hypotenuse = -GetAxisZ(object[n].getTotalRotation());
	vec3 opposite = -normalized(object[n].getPosition() - object[object[n].getOrbiting()].getPosition());
	opposite *= (cos(angle_from_two_vectors(hypotenuse, opposite))*length(hypotenuse));
	vec3 horizontal_dir = hypotenuse - opposite;

	double degrees_east = angle_from_two_vectors(horizontal_dir, east);
	double degrees_west = angle_from_two_vectors(horizontal_dir, west);
	double degrees = angle_from_two_vectors(horizontal_dir, north);

	if (degrees_east > degrees_west)
	{
		degrees *= -1;
	}

	return Mat4Yrotation(heading_degrees - degrees);
}

void showMasterClass::One_time_Initializations_pre()
{
	astrophysics_engine();

	if (start_synchronized) toggle_sync(true);

	for (int n = 0; n < total_objects; n++)
	{
		transform_object(n);
	}

	// Alinear los objetos que estan sobre la superficie a su direccion establecida
	for (int n = 0; n < total_objects; n++)
	{
		if (!object[n].getInOrbit())
		{
			Mat4d mat_4 = ground_rotation(n, object[n].getHeading());
			Mat3d mat_3;

			mat_3.r[0][0] = mat_4.r[0][0]; mat_3.r[1][0] = mat_4.r[1][0]; mat_3.r[2][0] = mat_4.r[2][0];
			mat_3.r[0][1] = mat_4.r[0][1]; mat_3.r[1][1] = mat_4.r[1][1]; mat_3.r[2][1] = mat_4.r[2][1];
			mat_3.r[0][2] = mat_4.r[0][2]; mat_3.r[1][2] = mat_4.r[1][2]; mat_3.r[2][2] = mat_4.r[2][2];

			quat rot_ = Mat3dToQuaternion(mat_3);
			object[n].setTotalRotation(~rot_*object[n].getTotalRotation());
			quat unrotated = quat{ 0.0, 0.0, 0.0, 1.0 } / object[object[n].getOrbiting()].getTotalRotation();
			object[n].setInheritedRotation(object[n].getTotalRotation() * unrotated);
		}		
	}
}

void showMasterClass::One_time_Initializations_post()
{
	for (int n = 0; n < total_objects; n++)
	{
		if (object[n].getAttachedTo() != "")
		{
			object[n].setLanded(false);

			// establecemos la posicion de acoplado inicial
			vec3 pos_ = GetAxisX(object[n].getTotalRotation()) * object[n].getDocked_position_b().x;
			pos_ += GetAxisY(object[n].getTotalRotation()) * object[n].getDocked_position_b().y;
			pos_ += GetAxisZ(object[n].getTotalRotation()) * object[n].getDocked_position_b().z;
			object[n].setPosition(object[n].getPosition() + pos_);

			// establecemos la rotacion de acoplado inicial
			auto_->RotateX(n, ToRadians_f(object[n].getDocked_rotation_x()));
			auto_->RotateY(n, ToRadians_f(object[n].getDocked_rotation_y()));
			auto_->RotateZ(n, ToRadians_f(object[n].getDocked_rotation_z()));

			object[n].setDockReady(true);
			object[n].setDockedTo(-1);
			object[n].setTopAbsoluteParent(-1);
			object[n].setTopArtificialParent(-1);
			for (int nn = 0; nn < total_objects; nn++)
			{
				if (object[n].getAttachedTo() == object[nn].getName()) script_->dock_object(n, nn, 20.f);
			}
		}
	}

	for (int n = 0; n < total_objects; n++)
	{
		if (object[n].getLanded())
		{
			if (object[control].getCameraPreferedAxis() == Vector3{ 0.f, 1.f, 0.f })camera->setOrientation({ 0.0f, 0.f, 0.f, 1.f });
			else if (object[control].getCameraPreferedAxis() == Vector3{ 0.f, 0.f, -1.f })camera->setOrientation({ 0.5f, 0.f, 0.f, 0.5f });

			// establecemos la rotacion de dockeado inicial
			auto_->RotateX(n, ToRadians_f(object[n].getDocked_rotation_x()));
			auto_->RotateY(n, ToRadians_f(object[n].getDocked_rotation_y()));
			auto_->RotateZ(n, ToRadians_f(object[n].getDocked_rotation_z()));

			object[n].setDockReady(true);
			object[n].setDockedTo(-1);
			object[n].setTopAbsoluteParent(-1);
			object[n].setTopArtificialParent(-1);
			script_->dock_object(n, object[n].getOrbiting(), 20.f);
		}
	}
}

void showMasterClass::set_bullet_scenario()
{
	//*
	int target = 49;

	object[39].setPosition(object[target].getPosition() + vec3{ -1000.0, 0, 0.0 });

	vec3 x_ = GetAxisX(object[39].getTotalRotation());
	vec3 y_ = GetAxisY(object[39].getTotalRotation());
	vec3 z_ = GetAxisZ(object[39].getTotalRotation());
	vec3 dir_ = { 0.0, 5.0, 20.0 };
	x_ = x_*dir_.x;
	y_ = y_*dir_.y;
	z_ = z_*dir_.z;
	dir_ = x_ + y_ + z_;

	object[40].setPosition(object[38].getPosition() + dir_);
	//vec3 vel = normalized(object[38].getPosition() - object[39].getPosition());
	//object[39].setVelocity(object[39].getVelocity() + vel*1000.);
	//*/
}

bool showMasterClass::add_object_to_bullet(int n)
{
	object[n].setBulletId(bullet_physics->getTotalObjects());
	object[n].setInBullet(true);
	if (!bullet_physics->create_object(n)) return false;

	return true;
}

bool showMasterClass::BulletPhysics_ObjectManager()
{
	bullet_physics->destroy_objects();

	for (int n = 0; n < total_objects; n++)
	{
		if (object[n].getType() == 4 && object[n].getCollisionShapeType() != -1)
		{			
			if (inside_range(n, bullet_range))
			{
				if (!add_object_to_bullet(n)) return false;
			}

			else if (object[n].getInBullet())
			{
				object[n].setBulletId(-1);
				object[n].setInBullet(false);
			}
		}
	}

	for (int n = 0; n < total_objects; n++)
	{
		if (object[n].getDockedTo() != -1)
		{
			int docket_to = object[n].getDockedTo();
			script_->undock_object(n);
			object[n].setBulletConstrained(true);

			if (!object[docket_to].getInBullet()) if (!add_object_to_bullet(docket_to)) return false;
			script_->dock_object(n, docket_to, float(object[n].getMass() + object[docket_to].getMass()));
		}
	}

	bullet_reload = false;

	return true;
}