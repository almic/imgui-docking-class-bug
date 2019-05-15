// Everything is from today's (5/14/2019) docking branch
// I changed nothing but removing some comments and added the docking code
// to dock the example windows, but leaving a floating one to test docking.

#include <imgui.h>
#include <imgui_internal.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int arc, char** argv)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Initialize OpenGL loader
	bool err = gladLoadGL() == 0;
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool dockspace_open = true;
	bool show_window1 = true;
	bool show_window2 = true;
	bool show_window3 = true;
	bool allow_unclassed = false;
	int floating_class = 0;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// DOCKING WINDOWS, VERY BASIC
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoNavFocus
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoTitleBar
			;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("MyDockspace", &dockspace_open, window_flags);
		ImGui::PopStyleVar(3);

		if (ImGui::DockBuilderGetNode(ImGui::GetID("MyDockspace")) == NULL)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id);

			ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
			ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.50f, NULL, &dock_main_id);

			ImGui::DockBuilderDockWindow("Docked First", dock_id_bottom);
			ImGui::DockBuilderDockWindow("Docked Last", dock_main_id);
			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode
		      | ImGuiDockNodeFlags_NoDockingInCentralNode
		      ;

		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		ImGui::End();


		// THE WINDOWS
		if (show_window1)
		{
			// To ensure default, unclassed window
			static ImGuiWindowClass floating_unclassed;
			floating_unclassed.ClassId = floating_class;
			floating_unclassed.DockingAllowUnclassed = true;
			ImGui::SetNextWindowClass(&floating_unclassed);
			ImGui::Begin("Floating Window");

			ImGui::Text("Try docking this into the windows, it won't work.");
			ImGui::Text("Then check \"Allow Unclassed Dock\" and suddenly you can dock everywhere!");
			
			if (floating_class == 0)
			{
				if (ImGui::Button("Make Same Class as First Docked"))
					floating_class = 8;
			}
			else
			{
				if (ImGui::Button("Make Unclassed"))
					floating_class = 0;
			}

			ImGui::Text("In fact, it seems like it checks the dockspace first, so technically it's all or nothing!");
			ImGui::End();
		}

		if (show_window2)
		{
			// Must be a different class, this is what prevents docking to this window specifically, which is what we want!
			// However, this prevents docking to all other windows, this is unintentional and unexpected!
			static ImGuiWindowClass last_window_class;
			last_window_class.DockingAllowUnclassed = allow_unclassed;
			last_window_class.ClassId = 5; // arbitrary value to check when debugging
			ImGui::SetNextWindowClass(&last_window_class);
			ImGui::Begin("Docked Last");

			if (allow_unclassed)
			{
				ImGui::Text("This window is allowing unclassed docks");
			}
			else
			{
				ImGui::Text("NO UNCLASSED DOCKING ALLOWED, everything breaks now.");
			}

			ImGui::Checkbox("Allow Unclassed Dock", &allow_unclassed);

			ImGui::End();
		}

		if (show_window3)
		{
			// This one will allow the unclassed floating window to dock, even though it's a different class.
			static ImGuiWindowClass first_window_class;
			first_window_class.DockingAllowUnclassed = true;
			first_window_class.ClassId = 8; // arbitrary value to check when debugging
			ImGui::SetNextWindowClass(&first_window_class);
			ImGui::Begin("Docked First");
			ImGui::Text("This window should always allow docking, but it doesn't!");
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
