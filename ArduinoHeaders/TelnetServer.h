/**	@defgroup	telnetserv
	@brief		TCP Telnet Service host
	@details	v 0.1
		Expects the ethernet to be configured and available.  Meaning Ethernet.begin(...)
		must already have succeeded, Ethernet.hardwareStatus() != EthernetNoHardware, and
		Ethernet.linkStatus() != LinkOFF

		There is no provision to close the server socket in the Arduino library.  This means 
		that once the service has started, it runs indefinitely.
*/

#ifndef __TELNET_SERVER
	#define __TELNET_SERVER

/***** Includes		*****/
	#include <Ethernet.h>

/***** Constants	*****/
	/**	@brief		The default port that will be used if no port setting is supplied */
	#define TELNET_DEFAULTPORT	23

	/**	@brief		Value placed in a client connection ID to indicate that this is not in use */
	#define	TELNET_IDINVALID	0
	
	/**	@brief		The first client ID that will be handed out, incrementing up from there*/*
	#define	TELNET_IDFIRST		1
	
	/**	@brief		Number of milliseconds a client can be idle before it is closed */
	#define TELNET_IDLETIMEOUT	30000

/***** Definitions	*****/
	typedef struct sTelnetClient_t {
		uint32_t nID;
		EthernetClient Client;
		uint32_t nBuffLen;
		uint32_t nRecvLen;
		char *pRecvBuff;
		uint32_t tLastActive;
		uint32_t tConnReq;
	} sTelnetClient_t;

	typedef void (*pfTelnetTextRecv_t)(uint32_t, const char *, uint32_t);

	typedef class cTelnetServer_t {
	public:
		cTelnetServer_t(uint16_t nPortNum, uint16_t nMaxClients);
		~cTelnetServer_t();
		bool Initialize();
		bool HandleConnections();
		bool SetTextReceiveCallback(pfTelnetTextRecv_t pfHandler);
		bool SendResponse(uint32_t nClientID, const char *pMessage);
		bool IsRunning();
		bool CloseClient(uint32_t nClientID);

	protected:
		virtual void TextReceived(uint32_t nClientID, const char *pMessage, uint32_t nMessageLen);

	private:
		uint32_t cnNextClientID;
		uint16_t cnTCPSocketNum;
		uint16_t cnMaxClients;

		pfTelnetTextRecv_t cpfTextRecvHandler;
		EthernetServer cEthServ = EthernetServer(TELNET_DEFAULTPORT);
		sTelnetClient_t *caEthClients;
		char *cpRecvBuff;

		bool ClientDataRecv(sTelnetClient_t *pClient, uint32_t nNumBytes);
	};

/***** Globals		*****/


/***** Prototypes 	*****/


/***** Functions	*****/
cTelnetServer_t::cTelnetServer_t(uint16_t nPortNum, uint16_t nMaxClients) {
	uint16_t nCtr;

	//Initialize class variables
	cnTCPSocketNum = nPortNum;
	cnMaxClients = nMaxClients;
	cnNextClientID = TELNET_IDFIRST; //ID zero is invalid, so start on 1

	cpRecvBuff = NULL;
	cpfTextRecvHandler = NULL;
	//caEthClients = (sTelnetClient_t *)malloc(sizeof(sTelnetClient_t) * (cnMaxClients + 1));
	caEthClients = new sTelnetClient_t [cnMaxClients];

	if (caEthClients != NULL) {
		for (nCtr = 0; nCtr < cnMaxClients; nCtr++) {
			caEthClients[nCtr].nID = TELNET_IDINVALID;
			caEthClients[nCtr].Client = EthernetClient();
			caEthClients[nCtr].nBuffLen = 0;
			caEthClients[nCtr].nRecvLen = 0;
			caEthClients[nCtr].pRecvBuff = NULL;
		}
	}

	return;
}

cTelnetServer_t::~cTelnetServer_t() {
	uint16_t nCtr;

	//Free all allocated resources
	for (nCtr = 0; caEthClients != NULL && nCtr < cnMaxClients; nCtr++) {
		if (caEthClients[nCtr].pRecvBuff != NULL) {
			delete caEthClients[nCtr].pRecvBuff;
			caEthClients[nCtr].pRecvBuff = NULL;
			caEthClients[nCtr].nBuffLen = 0;
		}
	}

	delete [] caEthClients;
	caEthClients = NULL;

	if (cpRecvBuff != NULL) {
		delete cpRecvBuff;
		cpRecvBuff = NULL;
	}

	return;
}

bool cTelnetServer_t::Initialize() {
	cEthServ = EthernetServer(cnTCPSocketNum); //Overwrite the server object with the correct port

	cEthServ.begin();

	if (cEthServ == true) { //listening for new clients
		return true;
	} else {
		return false;
	}
}

bool cTelnetServer_t::HandleConnections() {
	uint16_t nClientCtr, nFreeClientID;
	uint32_t nBytesAvail;
	uint32_t tCurrTime, tEllapsedTime;
	EthernetClient EthClient;

	tCurrTime = millis();

	//Look for a free client spot for incoming connections
	nFreeClientID = cnMaxClients;
	for (nClientCtr = 0; nClientCtr < cnMaxClients; nClientCtr++) {
		if (caEthClients[nClientCtr].nID == TELNET_IDINVALID) {
			nFreeClientID = nClientCtr;
			break;
		}
	}

	//CHeck for any incoming connections
	EthClient = cEthServ.accept();
	if (EthClient) { //A connection attempt was made
		if (nFreeClientID < cnMaxClients) { //Found a valid free spot
			caEthClients[nFreeClientID].Client = EthClient;

			//Received a new connection, put it in the list
			caEthClients[nFreeClientID].nID = cnNextClientID;
			caEthClients[nFreeClientID].tLastActive = millis();
			cnNextClientID += 1;

			if (cnNextClientID == TELNET_IDINVALID) {
				cnNextClientID = TELNET_IDFIRST; //Reset to first ID when invalid is reached
			}
		} else { //No connections available, close any connection requests
			EthClient.stop();
		}
	}

	//Loop through all connections and check for incoming data
	for (nClientCtr = 0; nClientCtr < cnMaxClients; nClientCtr++) {
		if (caEthClients[nClientCtr].nID == TELNET_IDINVALID) {
			//No connection in this spot, move along
			continue;
		}

		if ((!caEthClients[nClientCtr].Client) || (caEthClients[nClientCtr].Client.connected() == false)) {
			//This connection was lost, release it
			caEthClients[nClientCtr].Client.flush();
			caEthClients[nClientCtr].Client.stop();
			caEthClients[nClientCtr].nID = TELNET_IDINVALID;
		} else {
			nBytesAvail = caEthClients[nClientCtr].Client.available();

			if (nBytesAvail > 0) { //There are bytes waiting to be read
				caEthClients[nClientCtr].tLastActive = tCurrTime;
				caEthClients[nClientCtr].tConnReq = tCurrTime;
				ClientDataRecv(&caEthClients[nClientCtr], nBytesAvail);
			}
			
			nEllapsedTime = tCurrTime - caEthClients[nClientCtr].tLastActive;
			if (nEllapsedTime >= TELNET_IDLETIMEOUT) { //This connection has been idle too long, release it
				caEthClients[nClientCtr].Client.flush();
				caEthClients[nClientCtr].Client.stop();
				caEthClients[nClientCtr].nID = TELNET_IDINVALID;
			}
		}
	}

	return true;
}

bool cTelnetServer_t::SetTextReceiveCallback(pfTelnetTextRecv_t pfHandler) {
	cpfTextRecvHandler = pfHandler;

	return true;
}

bool cTelnetServer_t::SendResponse(uint32_t nClientID, const char *pMessage) {
	uint16_t nCtr;

	for (nCtr = 0; nCtr < cnMaxClients; nCtr++) {
		if (caEthClients[nCtr].nID == nClientID) {
			//Found the requested client, stop looking
			break;
		}
	}

	if (nCtr >= cnMaxClients) { //Unable to find the requested client
		return false;
	}

	//Transmit the data, assume the null terminator is in place
	caEthClients[nCtr].Client.println(pMessage);

	return true;
}

void cTelnetServer_t::TextReceived(uint32_t nClientID, const char *pMessage, uint32_t nMessageLen) {
	//Do no work, this is only intended for handling in an inheritting class
	return;
}

bool cTelnetServer_t::IsRunning() {
	if (cEthServ == true) { //listening for new clients
		return true;
	} else {
		return false;
	}
}

bool cTelnetServer_t::CloseClient(uint32_t nClientID) {
	uint16_t nCtr;

	for (nCtr = 0; nCtr < cnMaxClients; nCtr++) {
		if (caEthClients[nCtr].nID == nClientID) { //Found the client requested
			break;
		}
	}

	if (nCtr >= cnMaxClients) { //Never found the requested client
		return false;
	}

	caEthClients[nCtr].Client.flush();
	caEthClients[nCtr].Client.stop();

	caEthClients[nCtr].nID = TELNET_IDINVALID;
	caEthClients[nCtr].nRecvLen = 0;
	caEthClients[nCtr].nBuffLen = 0;

	if (caEthClients[nCtr].pRecvBuff != NULL) {
		delete caEthClients[nCtr].pRecvBuff;
	}
	caEthClients[nCtr].pRecvBuff = NULL;

	return true;
}

bool cTelnetServer_t::ClientDataRecv(sTelnetClient_t *pClient, uint32_t nNumBytes) {
	uint32_t nCtr, nStartIdx;
	char *pOldBuff;

	//Set the new size of the received data
	nStartIdx = pClient->nRecvLen; //Save off the current data length
	pClient->nRecvLen += nNumBytes;

	if ((pClient->nBuffLen == 0) || (pClient->pRecvBuff == NULL)) { //Clients buffer doesn't exist, create one
		pClient->pRecvBuff = new char[pClient->nRecvLen + 1]; //1 extra character for the null terminator
		pClient->nBuffLen = pClient->nRecvLen + 1;
	} else if (pClient->nBuffLen < pClient->nRecvLen + 1) { //Client buffer is too small, get a bigger one
		pOldBuff = pClient->pRecvBuff; //Save off the old buffer
		pClient->pRecvBuff = new char[pClient->nRecvLen + 1];

		if (pClient->pRecvBuff != NULL) { //No receive buffer exists
			memcpy(pClient->pRecvBuff, cpRecvBuff, nStartIdx + 1);
		}

		delete pOldBuff;
	}

	if (pClient->pRecvBuff == NULL) { //No receive buffer exists
		pClient->nBuffLen = 0;
		pClient->nRecvLen = 0;
		return false;
	}

	for (nCtr = 0; nCtr < nNumBytes; nCtr++) {
		pClient->pRecvBuff[nCtr + nStartIdx] = pClient->Client.read(); //Reads one byte of data
	}
	pClient->pRecvBuff[nCtr + nStartIdx] = '\0'; //Make sure the terminator exists

	if (pClient->pRecvBuff[pClient->nRecvLen - 1] != '\n') { //Command does not have a carraige return, keep collecting characters
		return true;
	}

	while ((pClient->pRecvBuff[pClient->nRecvLen - 1] == '\n') || (pClient->pRecvBuff[pClient->nRecvLen - 1] == '\r')) {
		pClient->pRecvBuff[pClient->nRecvLen - 1] = '\0'; //Remove the carraige return
		pClient->nRecvLen -= 1; //We took a character off (\n) reduce the length
	}

	//Assume the data is ascii and call the virtual TextReceived handler
	TextReceived(pClient->nID, pClient->pRecvBuff, pClient->nRecvLen);

	//if the call back is set, call that too
	if (cpfTextRecvHandler != NULL) {
		cpfTextRecvHandler(pClient->nID, pClient->pRecvBuff, pClient->nRecvLen);
	}

	//We've processed the command, reset for the next one
	pClient->pRecvBuff[0] = '\0';
	pClient->nRecvLen = 0;

	return true;
}

#endif
