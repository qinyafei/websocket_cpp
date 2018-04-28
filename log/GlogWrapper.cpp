#include "GlogWrapper.h"

#include <string>
#include <iostream>


#define GLOG_DIR "log"
#define MKLOGDIR "mkdir -p " GLOG_DIR


///捕捉 SIGSEGV 信号信息输出
void SigSEGVHandle(const char* err, int size)
{
	std::string str(err, size);

#ifdef CORE_TO_FILE
	std::ofstream ofs("glog_dump.log", std::ios::app);
	ofs << str;
	ofs.close();
#endif

	LOG(ERROR) << str;
}

/**
* @brief    在构造函数中，初始化glog，并配置参数
*/
GlogWrapper::GlogWrapper(const char* program)
{
	system(MKLOGDIR);
	google::InitGoogleLogging(program);

	//FLAGS_stderrthreshold = google::ERROR;
	FLAGS_minloglevel = google::INFO;
	//FLAGS_logtostderr = true;

	FLAGS_logtostderr = true;//输出到文件

  google::SetStderrLogging(google::INFO); //设置级别高于 google::ERROR 的日志同时输出到屏幕
	//google::SetStderrLogging(google::INFO); //设置级别高于 google::ERROR 的日志同时输出到屏幕
	FLAGS_colorlogtostderr = true;    //设置输出到屏幕的日志显示相应颜色
	google::SetLogDestination(google::INFO, GLOG_DIR"/INFO_"); //设置 google::INFO 级别的日志存储路径和文件名前缀
	google::SetLogDestination(google::WARNING, GLOG_DIR"/WARNING_");   //设置 google::WARNING 级别的日志存储路径和文件名前缀
	google::SetLogDestination(google::ERROR, GLOG_DIR"/ERROR_");   //设置 google::ERROR 级别的日志存储路径和文件名前缀
	FLAGS_logbufsecs = 0;        //缓冲日志输出，默认为30秒，此处改为立即输出
	FLAGS_max_log_size = 100;  //最大日志大小为 100MB
	FLAGS_stop_logging_if_full_disk = true;     //当磁盘被写满时，停止日志输出
	google::SetLogFilenameExtension("bstar_");     //设置文件名扩展，如平台？或其它需要区分的信息
	google::InstallFailureSignalHandler();      //捕捉 core dumped
	google::InstallFailureWriter(&SigSEGVHandle);    //默认捕捉 SIGSEGV 信号信息输出会输出到 stderr，可以通过下面的方法自定义输出>方式：

}


GlogWrapper::~GlogWrapper()
{
	google::ShutdownGoogleLogging();
}
