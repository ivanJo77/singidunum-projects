#pragma once
/*
  tools to work with Wininet.
*/

//The maximum amount of data allocated in memory for downloading.
#define WININET_MAXBYTES_TO_MEM  (10 * 1024 * 1024)

#include "mem.h"

namespace Wininet
{
  //Structure of CallURL.
  typedef struct
  {
    bool bAutoProxy;                   //Make an attempt to connect with WICF_USE_IE_PROXY, then without it, for each attempt.
    BYTE bTryCount;                    //Attempts at least be 1.
    DWORD dwRetryDelay;                //The delay between connections.
    HANDLE hStopEvent;                 //interrupt signal.
    LPSTR pstrUserAgent;               //UserAgent.
    LPSTR pstrURL;                     //URL.

    DWORD Connect_dwFlags;             //flags WICF_*. 

    LPSTR SendRequest_pstrReferer;     //Referrer
    void *SendRequest_pPostData;       //POST-data.
    DWORD SendRequest_dwPostDataSize;  //size of POST-data.
    DWORD SendRequest_dwFlags;         //flags WISRF_*.

    DWORD DownloadData_dwSizeLimit;    //Limit for downloading data.
    LPWSTR DownloadData_pstrFileName;  //If not NULL, then download the data in this file.
  }CALLURLDATA;

  //flags for _Connect.
  enum
  {
    WICF_USE_IE_PROXY = 0x1 //Whether to use a proxy to connect from the IE settings.
  };

  //flags of SendRequest.
  enum
  {
    WISRF_METHOD_GET      = 0x0, //Use the GET method. (DEFAULT)
    WISRF_METHOD_POST     = 0x1, //Use the POST method.
    WISRF_KEEP_CONNECTION = 0x2, //Close connection after the request. This option does not guarantee 
								 //the preservation of the connection, it depends on the server.
    WISRF_IS_HTTPS        = 0x4  //This query is performed by HTTPS protocol.
  };

  /*
    Init.
  */
  void Init(void);

  /*
    uninit.
  */
  void Uninit(void);

  /*
    Connecting to a server via HTTP/HTTPS protocol.

    IN pstrUserAgent - User-Agent, if NULL, the default value is used.
    IN pstrHost      - server address, IP or domain name.
    IN wPort         - server port.
    IN dwFlags       - flags WICF_*.

    Return           - Connection handle or NULL.
  */
  HINTERNET _Connect(LPSTR pstrUserAgent, LPSTR pstrHost, WORD wPort, DWORD dwFlags);

  /*
    Closing a connection that was opened with _Connect.

    IN hConnect - Connection handle.
  */
  void _CloseConnection(HINTERNET hConnect);

  /*
    Closing the handle, and any parents.

    IN OUT handle - handle.
  */
  void _closeWithParents(HINTERNET handle);

  /*
    Calling InternetStatusCallback.

    ... -  according InternetStatusCallback.

    Return - true - the function has been called,
             false - the function has not been called.
  */
  bool callCallback(HINTERNET handle, DWORD_PTR context, DWORD internetStatus, LPVOID statusInformation, DWORD statusInformationLength);

  /*
    Send request.

    IN hConnect       - connection handle.
    IN pstrURI        - the requested object.
    IN pstrReferer    - referrer or NULL.
    IN pPostData      - POST-data or NULL.
    IN dwPostDataSize - size of pPostData.
    IN dwFlags        - flags WISRF_*.

    Return            - in case of success handle received from HttpOpenRequest, 
						or NULL in case of error.
  */
  HINTERNET _SendRequest(HINTERNET hConnect, LPSTR pstrURI, LPSTR pstrReferer, void *pPostData, DWORD dwPostDataSize, DWORD dwFlags);

  /*
    Download data.

    IN hRequest    - request handle.
    OUT pBuf       - buffer for the data, after use remove it with Mem. Can be NULL.
    IN dwSizeLimit - Downloaded limit bytes, if 0 or greater then WININET_MAXBYTES_TO_MEM, then
                     dwSizeLimit =  WININET_MAXBYTES_TO_MEM;
    IN hStopEvent  - handle for interrupt events for downloading or NULL.

    Return         - true - in case of success,
                     false - in case of error, including fires StopEvent and exceeds dwSizeLimit. 
					 If an error occurs, the connection must be closed.
  */
  bool _DownloadData(HINTERNET hRequest, MEMDATA *pBuf, DWORD dwSizeLimit, HANDLE hStopEvent);
  
  /*
    Download data into file.

    IN hRequest     - request handle.
    IN pstrFileName - file name to save downloaded data.
    IN dwSizeLimit  - limit of bytes downloaded, if is 0, then there is no limit.
    IN hStopEvent   - handle for interrupt events for downloading or NULL.

    Return          - true - in case of success,
					  false - in case of error, including fires StopEvent and exceeds dwSizeLimit. 
					  If an error occurs, the connection must be closed.
  */
  bool _DownloadDataToFile(HINTERNET hRequest, LPWSTR pstrFileName, DWORD dwSizeLimit, HANDLE hStopEvent);

  /*
    A wrapper for _Connect + _SendRequest + _DownloadData(ToFile).

    IN pcud  - URL parameters;
    OUT pBuf - buffer for the data, to be removed after use by Mem. May be NULL.
               If pcud->DownloadData_pstrFileName != NULL then it is not used.

    Return   - true - in case of success,
               false - in case of error.
  */
  bool _CallURL(CALLURLDATA *pcud, MEMDATA *pBuf);

  /*
    Retrieve the User-Agent from Internet Explorer.

    Return - User-Agent, or NULL in case of error. The memory must be freed by Mem.
  */
  LPSTR _GetIEUserAgent(void);

  /*
    Speed ​​test of download URL.

    IN url       - URL.
    IN stopEvent - EventStop function or NULL.

    Return       - Spent time in ms, or 0 - in case of error.
  */
  DWORD _testDownloadDelay(LPSTR url, HANDLE stopEvent);

  /*
    Extension of InternetQueryOptionA with automatic memory allocation.

    IN internet - handle.
    IN option   - options.
    OUT length  - data length. If it is a string, the number will be returned not including null character.

	Return      - data (need to release by Mem), if it is a string, it will be ending in
				  null character. Or NULL in case of error.
  */
  void *_queryOptionExA(HINTERNET internet, DWORD option, LPDWORD length);

  /*
    Extension of InternetQueryOptionW with automatic memory allocation.

	IN internet - handle.
	IN option   - options.
	OUT length  - data length. If it is a string, the number will be returned not including null character.

	Return      - data (need to release by Mem), if it is a string, it will be ending in
	null character. Or NULL in case of error.
  */
  void *_queryOptionExW(HINTERNET internet, DWORD option, LPDWORD length);

  /*
    Extension of HttpQueryInfoA with automatic memory allocation.

    IN internet  - handle.
    IN infoLevel - flags indicating the data to obtain.
    OUT length   - the size of the string without the null character.

	Return       - line ending in the null character (must be freed by Mem),
				   or NULL in case of error.
  */
  LPSTR _queryInfoExA(HINTERNET request, DWORD infoLevel, LPDWORD length, LPDWORD index);
};
