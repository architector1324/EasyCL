#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <string>
#include <map>
#include <vector>

namespace ecl {
	class ErrorObject {
	protected:
		std::vector<std::string> error; // вектор ошибок
		int error_code; // код последней ошибки
		bool has_error; // содержит ли вектор ошибок ошибки (сообщение ok не считается за ошибку)
	public:
		void setError(std::string error);
		// разместить сообщение об ошибке в вектор ошибок
		int getErrorCode(); // получить код последней ошибки

		void clearErrors(); // очистить вектор ошибок
		std::string getError(); // получить конечное сообщение обо всех ошибках в векторе ошибок
		std::vector<std::string>& getErrorVector(); // получить вектор ошибок
		bool HasError(); // узнать, содержит ли вектор ошибок ошибки
	};

	// аналогично ErrorObject
	class SharedErrorObject { 
	protected:
		static std::vector<std::string> shared_error;
		static int shared_error_code;
		static bool has_shared_error;
	public:
		static void setSharedError(std::string error);
		static int getSharedErrorCode();

		static void clearSharedErrors();
		static std::string getSharedError();
		static std::vector<std::string>& getSharedErrorVector();
		static bool HasSharedError();
	};

	class Error {
	private:
		std::string prefix; // префикс ошибки - указывается источник ошибки
	public:
		Error(std::string prefix);
		bool checkError(ErrorObject* obj); // проверить ошибки в обьекте obj
		bool checkSharedError();
		void setError(ErrorObject* obj, std::string error_str); // разместить сообщение об ошибке в обьекте obj
		void setSharedError(std::string error_str);
		virtual std::string getError(int error) = 0; // получить сообщение об ошибке по ее коду
	};

	class CLError : public Error {
	public:
		CLError(std::string prefix);
		std::string getError(int error);
	};

	class GPGPUError : public Error {
	public:
		GPGPUError(std::string prefix);
		std::string getError(int error);
	};

	class Platform : public SharedErrorObject {
	private:
		static cl_uint platforms_count; // количество opencl платформ в системе
		static cl_platform_id* platforms;

		static cl_uint* devices_count; // количество вычислительных устройств в каждой платформе
		static cl_device_id** devices;

		static bool initialized; // флаг инициализации платформ
	public:
		static std::string init(cl_device_type type);
		static bool Initialized(); // получить флаг инициализации

		static cl_platform_id* getPlatform(size_t i); // получить платформу
		static cl_device_id* getDevice(size_t platform_i, size_t i); // получить устройство

		virtual void abstract() = 0; // данный класс является абстрактным
	};


	class GPUArgument : public ErrorObject {
	private:
		std::map<cl_context*, cl_mem> buffer; // карта буферов по контексту
		void* ptr; // указатель на адрес значения или массива
		size_t arr_size; // размер массива * размер типа его элементов
		cl_mem_flags mem_type; // тип использумой памяти
	public:
		GPUArgument(void* ptr, size_t arr_size, cl_mem_flags mem_type);
		std::string checkBuffer(cl_context* context); // проверить buffer на контекст

		void* getPtr(); // получить указатель
		size_t getArrSize(); // получить размер массива * размер типа его элементов
		cl_mem* getArgument(cl_context* context); // получить указатель на буфер по контексту

		void setPtr(void* ptr); // задать новый указатель на массив
		void setArrSize(size_t arr_size); // задать новый размер массива
		void setMemType(cl_mem_flags mem_type); // задать новый тип памяти

		~GPUArgument();
	};

	class GPUFunction : public ErrorObject {
	private:
		std::map<cl_program*, cl_kernel> function; // карта ядер по программам
		const char* name; // имя ядра
	public:
		GPUFunction(const char* name);
		std::string checkKernel(cl_program* program); // проверить ядро на программу
		cl_kernel* getFunction(cl_program* program); // получить указатель на программу
		~GPUFunction();
	};

	class GPUProgram : public ErrorObject {
	private:
		std::map<cl_context*, cl_program> program; // карта программ по контексту
		const char* program_source; // исходный текст программы
		size_t program_source_length; // длина исходного текста программы

		std::string getBuildError(cl_context* context, cl_device_id* device); // получить сообщение об ошибке построения программы
	public:
		GPUProgram(const char* source, size_t length);
		GPUProgram(std::string& source);
		std::string checkProgram(cl_context* context, cl_device_id* device); // проверить программу на контекст
		cl_program* getProgram(cl_context* context); // получить указатель на программу
		~GPUProgram();
	};

	class GPU : public ErrorObject {
	private:
		cl_device_id* device; // указатель на устройство
		
		cl_context context; // opencl контекст
		cl_command_queue queue; // opencl очередь запросов
	public:
		GPU(size_t platform_index, size_t device_index);
		std::string sendData(const std::vector<GPUArgument*>& args); // отправить данные на устройство
		// выполнить программу на устройстве
		std::string compute(GPUProgram* prog, GPUFunction* func, const std::vector<GPUArgument*>& args, const std::vector<size_t>& global_work_size, size_t dim); 
		std::string receiveData(const std::vector<GPUArgument*>& args); // получить данные с устройства
		~GPU();
	};
}

