#pragma once

#define _WINSOCKAPI_
#include "CoreMinimal.h"
#include <HAL/Runnable.h>
#include <Memory>
#include "Components/ActorComponent.h"
#include "Packet/PacketInformation.h"

THIRD_PARTY_INCLUDES_START
#pragma push_macro("check")
#undef check

#include <Functions/Log/Log.hpp>
#include <NetworkModel/IOCP/IOCP.hpp>

#pragma pop_macro("check")
THIRD_PARTY_INCLUDES_END

#include "IOCPComponent.generated.h"

class STARWARS_API IOCPPacketProcessThread : public FRunnable {
public:
	IOCPPacketProcessThread(SERVER::NETWORKMODEL::IOCP::IOCP* pIOCPInstance, FThreadSafeBool& bThreadRunState);
	~IOCPPacketProcessThread();

	bool Init() override;
	uint32 Run() override;
	void Stop() override;

private:
	SERVER::NETWORKMODEL::IOCP::IOCP* m_pIOCPInstance;

	FRunnableThread* m_workerThread;

	FThreadSafeBool& m_bThreadRunState;

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STARWARS_API UIOCPComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UIOCPComponent();

	SERVER::NETWORKMODEL::BASEMODEL::PACKETPROCESSOR m_packetProcessorList;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void FinishDestroy() override;

private:
	SERVER::FUNCTIONS::SOCKETADDRESS::SocketAddress m_serverAddress;

	SERVER::NETWORKMODEL::IOCP::IOCP* m_pIOCP;
	IOCPPacketProcessThread* m_pIOCPPacketProcessor;

	FThreadSafeBool m_bThreadRunState;

};