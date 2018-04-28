#include "GlogWrapper.h"

#include <string>
#include <iostream>


#define GLOG_DIR "log"
#define MKLOGDIR "mkdir -p " GLOG_DIR


///��׽ SIGSEGV �ź���Ϣ���
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
* @brief    �ڹ��캯���У���ʼ��glog�������ò���
*/
GlogWrapper::GlogWrapper(const char* program)
{
	system(MKLOGDIR);
	google::InitGoogleLogging(program);

	//FLAGS_stderrthreshold = google::ERROR;
	FLAGS_minloglevel = google::INFO;
	//FLAGS_logtostderr = true;

	FLAGS_logtostderr = true;//������ļ�

  google::SetStderrLogging(google::INFO); //���ü������ google::ERROR ����־ͬʱ�������Ļ
	//google::SetStderrLogging(google::INFO); //���ü������ google::ERROR ����־ͬʱ�������Ļ
	FLAGS_colorlogtostderr = true;    //�����������Ļ����־��ʾ��Ӧ��ɫ
	google::SetLogDestination(google::INFO, GLOG_DIR"/INFO_"); //���� google::INFO �������־�洢·�����ļ���ǰ׺
	google::SetLogDestination(google::WARNING, GLOG_DIR"/WARNING_");   //���� google::WARNING �������־�洢·�����ļ���ǰ׺
	google::SetLogDestination(google::ERROR, GLOG_DIR"/ERROR_");   //���� google::ERROR �������־�洢·�����ļ���ǰ׺
	FLAGS_logbufsecs = 0;        //������־�����Ĭ��Ϊ30�룬�˴���Ϊ�������
	FLAGS_max_log_size = 100;  //�����־��СΪ 100MB
	FLAGS_stop_logging_if_full_disk = true;     //�����̱�д��ʱ��ֹͣ��־���
	google::SetLogFilenameExtension("bstar_");     //�����ļ�����չ����ƽ̨����������Ҫ���ֵ���Ϣ
	google::InstallFailureSignalHandler();      //��׽ core dumped
	google::InstallFailureWriter(&SigSEGVHandle);    //Ĭ�ϲ�׽ SIGSEGV �ź���Ϣ���������� stderr������ͨ������ķ����Զ������>��ʽ��

}


GlogWrapper::~GlogWrapper()
{
	google::ShutdownGoogleLogging();
}
