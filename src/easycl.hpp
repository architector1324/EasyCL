#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <cstring>
#include <string>
#include <map>
#include <vector>

//Copyright (c) Architector 2018, gluhoy1324@gmail.com
//This file is part of EasyCL.

//    EasyCL is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    EasyCL is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with EasyCL.  If not, see <https://www.gnu.org/licenses/>.

namespace ecl {

    class Initializable{
    protected:
        bool initialized; // флаг инициализации
    public:
        bool Initialized() const;
    };

    class SharedInitializable{
    protected:
       static bool shared_initialized;
    public:
       static bool SharedInitialized();
    };

	class ErrorObject {
	protected:
        std::vector<std::string> error_vector; // вектор ошибок
        std::string error = ""; // конечное сообщение об ошибке
        int error_code = 0; // код последней ошибки
        bool has_error = false; // содержит ли вектор ошибок ошибки (сообщение ok не считается за ошибку)
        void generateError();
    public:
		void setError(std::string error);
		// разместить сообщение об ошибке в вектор ошибок
        int getErrorCode() const; // получить код последней ошибки

		void clearErrors(); // очистить вектор ошибок
        const std::string &getError() const; // получить конечное сообщение обо всех ошибках в векторе ошибок
        const std::vector<std::string>& getErrorVector() const; // получить вектор ошибок
        bool HasError() const; // узнать, содержит ли вектор ошибок ошибки
	};

	// аналогично ErrorObject
	class SharedErrorObject { 
	protected:
        static std::vector<std::string> shared_error_vector;
        static std::string shared_error;
        static int shared_error_code;
        static bool has_shared_error;
        static void generateSharedError();
	public:
		static void setSharedError(std::string error);
        static int getSharedErrorCode();

		static void clearSharedErrors();
        static std::string& getSharedError();
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
        virtual const char* getError(int error) const = 0; // получить сообщение об ошибке по ее коду
	};

	class CLError : public Error {
	public:
		CLError(std::string prefix);
        const char* getError(int error) const;
	};

	class GPGPUError : public Error {
	public:
		GPGPUError(std::string prefix);
        const char* getError(int error) const;
	};

    class Platform : public SharedErrorObject, public SharedInitializable {
	private:
		static cl_uint platforms_count; // количество opencl платформ в системе
		static cl_platform_id* platforms;

		static cl_uint* devices_count; // количество вычислительных устройств в каждой платформе
		static cl_device_id** devices;
	public:
        static std::string& init(cl_device_type type);

        static void checkPlatform(size_t platform_index);
        static void checkDevice(size_t platform_index, size_t device_index);

		static cl_platform_id* getPlatform(size_t i); // получить платформу
		static cl_device_id* getDevice(size_t platform_i, size_t i); // получить устройство
        static cl_uint getPlatformsCount();

		virtual void abstract() = 0; // данный класс является абстрактным
	};

	class GPUArgument : public ErrorObject {
	private:
		std::map<cl_context*, cl_mem> buffer; // карта буферов по контексту
        void* ptr; // указатель на адрес значения или массива
        size_t arr_size; // размер массива * размер типа его элементов
		cl_mem_flags mem_type; // тип использумой памяти
	public:
        GPUArgument();
        GPUArgument(const void* ptr, size_t arr_size);
        GPUArgument(void* ptr, size_t arr_size, cl_mem_flags mem_type = CL_MEM_READ_WRITE);
        const std::string& checkBuffer(cl_context* context); // проверить buffer на контекст

        void* getPtr() const; // получить указатель
        size_t getArrSize() const; // получить размер массива * размер типа его элементов
        const cl_mem* getArgument(cl_context* context) const; // получить указатель на буфер по контексту

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
        const std::string &checkKernel(cl_program* program); // проверить ядро на программу
        const cl_kernel *getFunction(cl_program* program) const; // получить указатель на программу
		~GPUFunction();
	};

	class GPUProgram : public ErrorObject {
	private:
		std::map<cl_context*, cl_program> program; // карта программ по контексту
		const char* program_source; // исходный текст программы
		size_t program_source_length; // длина исходного текста программы

        const char* getBuildError(cl_context* context, cl_device_id* device); // получить сообщение об ошибке построения программы
	public:
        GPUProgram(const char* source);
		GPUProgram(const char* source, size_t length);
		GPUProgram(std::string& source);
        const std::string &checkProgram(cl_context* context, cl_device_id* device); // проверить программу на контекст
        const cl_program* getProgram(cl_context* context) const; // получить указатель на программу
		~GPUProgram();
	};

    class GPU : public ErrorObject, public Initializable {
	private:
		cl_device_id* device; // указатель на устройство
		
		cl_context context; // opencl контекст
		cl_command_queue queue; // opencl очередь запросов
	public:
		GPU(size_t platform_index, size_t device_index);
        const std::string& sendData(const std::vector<GPUArgument*>& args); // отправить данные на устройство
		// выполнить программу на устройстве
        const std::string& compute(GPUProgram* prog, GPUFunction* func, const std::vector<GPUArgument*>& args, const std::vector<size_t>& global_work_size);
        const std::string& receiveData(const std::vector<GPUArgument*>& args); // получить данные с устройства

		~GPU();
	};
}
