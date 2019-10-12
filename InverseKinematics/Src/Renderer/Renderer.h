#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace IK {

	enum BufferType {
		Vertex,
		Index,
		Uniform,
		Generic,
	};
	class Buffer {
	public:
		virtual bool Init(size_t size, BufferType type, void* data = nullptr);
		~Buffer();
		void Update(void* data, size_t size);
		GLuint GetId() const { return id; }
		BufferType GetType() const { return type; }
	private:
		void Destroy();
		BufferType type;
		GLuint id;
	};
	class ResourceBuffer : public Buffer {
	public:
		void Bind(size_t bindingPoint);
	};

	class Shader {
	public:
		bool LoadShader(const char* vsFile, const char* fsFile);
		void Use();
		void UniformBlockBinding(const char* blockName, GLuint binding);
		~Shader();
	private:
		GLuint id;
	};

	class VAO {
	public:
		bool Init(GLuint vbo, GLuint ibo);
		void SetAttribute(GLuint index, GLint size, GLenum type, GLboolean normalize,GLsizei stride, GLsizei offset);
		void Bind();
		void UnBind();
		~VAO();
	private:
		GLuint id;
	};

	enum Key {
		W, A, S, D,
		Q, E,
		Space,
		Enter,
		Esc,
		None,
		KeyNum,
	};


	struct KeyCallbackBase {
		virtual ~KeyCallbackBase() = default;
		virtual void Invoke(Key key, bool action) = 0;
	};
	struct KeyCallback {
		static void Invoke(GLFWwindow* window, int key, int scancode, int action, int mod);

		static KeyCallbackBase* callback;
	};

	template<typename T>
	struct TemplateKeyCallback : KeyCallbackBase {
		TemplateKeyCallback(T t) : callback(t) {}
		void Invoke(Key key, bool action) override {
			callback(key, action);
		}
		T callback;
	};

	struct MouseCallbackBase {
		virtual ~MouseCallbackBase() = default;
		virtual void Invoke(double x, double y) = 0;
	};
	struct MouseCallback {
		static void Invoke(GLFWwindow* window, double x, double y);
		static MouseCallbackBase* callback;
	};
	template<typename T>
	struct TemplateMouseCallback : MouseCallbackBase {
		TemplateMouseCallback(T t) : callback(t) {}
		void Invoke(double x, double y) override {
			callback(x, y);
		}
		T callback;
	};

	class Renderer {
	public:
		static Renderer& GetInstance();
		bool Init(int width, int height, const char* title);

		bool WindowShouldClose() const;
		void ClearBuffer();
		void SwapBuffers() const;

		void CloseWindow();
		void SetMousePos(double x, double y);
		void SetCursorVisible(bool);

		template<typename T>
		void SetKeyCallback(T t) {
			delete KeyCallback::callback;
			KeyCallback::callback = new TemplateKeyCallback<T>(t);
		}
		template<typename T>
		void SetMouseCallback(T t) {
			delete MouseCallback::callback;
			MouseCallback::callback = new TemplateMouseCallback<T>(t);
		}
	private:
		Renderer() = default;
		~Renderer();
		GLFWwindow * window;
		bool windowDestroyed = false;
		bool initialized = false;
	};
}// namespace IK