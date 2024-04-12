#include "IOCPComponent.h"
#include <Engine/World.h>

UIOCPComponent::UIOCPComponent() : m_serverAddress("172.30.1.27", 3550), m_pIOCP(nullptr), m_pIOCPPacketProcessor(nullptr) {
	SERVER::FUNCTIONS::LOG::Log::SetLogFileDirectory(L"D:\\Unreal Projects\\StarWars\\Log\\");
	PrimaryComponentTick.bCanEverTick = false;
}

void UIOCPComponent::BeginPlay() {
	Super::BeginPlay();

	m_pIOCP = new SERVER::NETWORKMODEL::IOCP::IOCP(m_packetProcessorList, 6);
	m_pIOCP->Initialize(SERVER::NETWORK::PROTOCOL::UTIL::BSD_SOCKET::EPROTOCOLTYPE::EPT_BOTH, m_serverAddress);

	m_pIOCPPacketProcessor = new IOCPPacketProcessThread(m_pIOCP, m_bThreadRunState);
}

void UIOCPComponent::BeginDestroy() {
	Super::BeginDestroy();

	m_bThreadRunState = false;
}

void UIOCPComponent::FinishDestroy() {
	Super::FinishDestroy();

	if (m_pIOCPPacketProcessor) {
		m_pIOCPPacketProcessor->Stop();
		delete m_pIOCPPacketProcessor;
	}

	if (m_pIOCP) {
		m_pIOCP->Destroy();
		delete m_pIOCP;
	}
}


IOCPPacketProcessThread::IOCPPacketProcessThread(SERVER::NETWORKMODEL::IOCP::IOCP* pIOCPInstance, FThreadSafeBool& bThreadRunState) : m_pIOCPInstance(pIOCPInstance), m_bThreadRunState(bThreadRunState) {
	m_bThreadRunState.AtomicSet(true);

	m_workerThread = (FRunnableThread::Create(this, TEXT("Packet Process Worker Thread")));
}

IOCPPacketProcessThread::~IOCPPacketProcessThread() {
	if (m_workerThread)
		m_workerThread->Kill();
}

bool IOCPPacketProcessThread::Init() {
	return true;
}

uint32 IOCPPacketProcessThread::Run() {
	while (m_bThreadRunState) {
		if (m_pIOCPInstance)
			m_pIOCPInstance->Run();

		FPlatformProcess::Sleep(0.001f);
	}
	return uint32();
}

void IOCPPacketProcessThread::Stop() {
	m_pIOCPInstance = nullptr;
}