#include "Renderer.h"
#include <vector>
#include <iostream>

namespace IK {

	static GLenum bufferType[] = {
		GL_ARRAY_BUFFER,
		GL_ELEMENT_ARRAY_BUFFER,
		GL_UNIFORM_BUFFER,
		GL_SHADER_STORAGE_BUFFER,
	};

	bool Buffer::Init(size_t size, BufferType type, void* data) {
		Destroy();
		glGenBuffers(1, &id);
		glBindBuffer(bufferType[type], id);
		glBufferData(bufferType[type], size, data, GL_DYNAMIC_DRAW);
		glBindBuffer(bufferType[type], 0);
		this->type = type;
		return true;
	}

	Buffer::~Buffer() {
		Destroy();
	}

	void Buffer::Update(void* data, size_t size) {
		glBindBuffer(bufferType[type], id);
		glBufferSubData(bufferType[type], 0, size, data);
		glBindBuffer(bufferType[type], 0);
	}

	void Buffer::Destroy() {
		if (id) {
			glDeleteBuffers(1, &id);
		}
	}


	void ResourceBuffer::Bind(size_t bindingPoint) {
		GLenum target = bufferType[GetType()];
		glBindBufferBase(target, bindingPoint, GetId());
	}

	bool ReadFile(const char* filename, std::vector<char>& buf) {
		struct stat st;
		if (stat(filename, &st)) {
			return false;
		}
		FILE* fp = fopen(filename, "rb");
		if (!fp) {
			return false;
		}
		buf.resize(st.st_size + 1);
		const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
		fclose(fp);
		if (readSize != st.st_size) {
			return false;
		}
		buf.back() = '\0';
		return true;
	}

	GLuint CompileShader(GLenum type, const GLchar* string) {
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &string, nullptr);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				std::vector<char> buf;
				buf.resize(infoLen);
				if (static_cast<int>(buf.size()) >= 0) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
					std::cerr << "ERROR: シェーダのコンパイルに失敗\n" << buf.data() << std::endl;
				}
			}
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}

	GLuint CreateShaderProgram(const GLchar* vsCode, const GLchar* fsCode) {
		GLuint vs = CompileShader(GL_VERTEX_SHADER, vsCode);
		GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsCode);
		if (!vs || !fs) {
			return 0;
		}
		GLuint program = glCreateProgram();
		glAttachShader(program, fs);
		glDeleteShader(fs);
		glAttachShader(program, vs);
		glDeleteShader(vs);
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint infoLen = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				std::vector<char> buf;
				buf.resize(infoLen);
				if (static_cast<int>(buf.size()) >= infoLen) {
					glGetProgramInfoLog(program, infoLen, NULL, buf.data());
					std::cerr << "ERROR: シェーダのリンクに失敗\n" << buf.data() << std::endl;
				}
			}
			glDeleteProgram(program);
			return 0;
		}
		return program;
	}

	GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFilename) {
		std::vector<char> vsBuf;
		if (!ReadFile(vsFilename, vsBuf)) {
			std::cerr << "ERROR in Shader::CreateProgramFromFile: \n" <<
				vsFilename << "を読み込めません。" << std::endl;
			return 0;
		}
		std::vector<char> fsBuf;
		if (!ReadFile(fsFilename, fsBuf)) {
			std::cerr << "ERROR in Shader::CreateProgramFromFile: \n" <<
				fsFilename << "を読み込めません。" << std::endl;
			return 0;
		}
		return CreateShaderProgram(vsBuf.data(), fsBuf.data());
	}

	bool Shader::LoadShader(const char * vsFile, const char * fsFile) {
		id = CreateProgramFromFile(vsFile, fsFile);
		if (!id) {
			return false;
		}

		return true;
	}

	void Shader::Use() {
		glUseProgram(id);
	}

	void Shader::UniformBlockBinding(const char* blockName, GLuint binding) {
		GLuint block = glGetUniformLocation(id, blockName);
		glUniformBlockBinding(id, block, binding);
	}

	Shader::~Shader() {
		if (id) {
			glDeleteProgram(id);
		}
	}

	bool VAO::Init(GLuint vbo, GLuint ibo) {
		glGenVertexArrays(1, &id);
		glBindVertexArray(id);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBindVertexArray(0);

		return true;
	}

	void VAO::SetAttribute(GLuint index, GLint size, GLenum type, GLboolean normalize, GLsizei stride, GLsizei offset) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, type, normalize, stride, reinterpret_cast<GLvoid*>(offset));
	}

	void VAO::Bind() {
		glBindVertexArray(id);
	}

	void VAO::UnBind() {
		glBindVertexArray(0);
	}

	VAO::~VAO() {
		if (id) {
			glDeleteVertexArrays(1, &id);
		}
	}


	Key Convert(int key) {
		switch (key) {
		case GLFW_KEY_W: return Key::W;
		case GLFW_KEY_A: return Key::A;
		case GLFW_KEY_S: return Key::S;
		case GLFW_KEY_D: return Key::D;
		case GLFW_KEY_Q: return Key::Q;
		case GLFW_KEY_E: return Key::E;
		case GLFW_KEY_SPACE: return Key::Space;
		case GLFW_KEY_ENTER: return Key::Enter;
		case GLFW_KEY_ESCAPE: return Key::Esc;
		default:
			return Key::None;
		}
	}

	void KeyCallback::Invoke(GLFWwindow* window, int key, int scancode, int action, int mod) {
		if (callback) {

			callback->Invoke(Convert(key), (action != GLFW_RELEASE ? true : false));
		}
	}
	KeyCallbackBase* KeyCallback::callback = nullptr;

	void MouseCallback::Invoke(GLFWwindow* window, double x, double y) {
		if (callback) {
			callback->Invoke(x, y);
		}
	}
	MouseCallbackBase* MouseCallback::callback = nullptr;


	Renderer & Renderer::GetInstance() {
		static Renderer instance;
		return instance;
	}

	bool Renderer::Init(int width, int height, const char * title) {
		if (initialized) {
			return true;
		}

		if (glfwInit() != GL_TRUE) {
			return false;
		}
		initialized = true;

		window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!window) {
			return false;
		}

		glfwMakeContextCurrent(window);

		if (glewInit() != GLEW_OK) {
			return false;
		}

		//Callback
		glfwSetKeyCallback(window, KeyCallback::Invoke);
		glfwSetCursorPosCallback(window, MouseCallback::Invoke);

		return true;
	}

	bool Renderer::WindowShouldClose() const {
		return windowDestroyed || glfwWindowShouldClose(window);
	}

	void Renderer::ClearBuffer() {
		glClearColor(0.1, 0.3, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	void Renderer::SwapBuffers() const {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	void Renderer::CloseWindow() {
		glfwDestroyWindow(window);
		windowDestroyed = true;
	}

	void Renderer::SetMousePos(double x, double y) {
		glfwSetCursorPos(window, x, y);
	}

	void Renderer::SetCursorVisible(bool f) {
		if (f) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
	}

	Renderer::~Renderer() {
		if (initialized) {
			glfwTerminate();
		}
		delete KeyCallback::callback;
		delete MouseCallback::callback;
	}

} // namespace IK