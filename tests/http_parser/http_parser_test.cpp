#include <HTTP.h>
#include <Logger.h>
#include <string>
#include <iostream>

/*
const std::string postreq = "POST /enlighten/calais.asmx HTTP/1.1\r\n\
Host: api.opencalais.com\r\n\
Content-Type: text/xml; charset=utf-8\r\n\
Content-Length: length\r\n\
SOAPAction: \"http://clearforest.com/Enlighten\"\r\n\r\n";
*/

const std::string postreq = "POST /enlighten/calais.asmx HTTP/1.1\r\n\
Host: api.opencalais.com\r\n\
Content-Type: text/xml; charset=utf-8\r\n\
SOAPAction: \"http://clearforest.com/Enlighten\"\r\n\r\n";

const std::string getreqparams = "GET /enlighten/calais.asmx/Enlighten?licenseID=string&content=string&paramsXML=string HTTP/1.1\r\n\
Host: api.opencalais.com\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\r\n";

const std::string postreqparams = "POST /enlighten/calais.asmx/Enlighten HTTP/1.1\r\n\
Host: api.opencalais.com\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
Content-Length: 50\r\n\r\n\
licenseID=string&content=string&paramsXML=string";

const std::string postreqparams1 = "POST /datatables/DataTables/examples/server_side/scripts/post.php HTTP/1.1\r\n\
X-Requested-With: XMLHttpRequest\r\n\
User-Agent: Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_8; en-gb) AppleWebKit/533.21.1 (KHTML, like Gecko) Version/5.0.5 Safari/533.21.1\r\n\
Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n\
Accept: application/json, text/javascript, */*; q=0.01\r\n\
Accept-Language: en-gb\r\n\
Accept-Encoding: gzip, deflate\r\n\
Content-Length: 507\r\n\
Connection: keep-alive\r\n\r\n\
sEcho=2&iColumns=5&sColumns=&iDisplayStart=0&iDisplayLength=10&mDataProp_0=0&mDataProp_1=1&mDataProp_2=2&mDataProp_3=3&mDataProp_4=4&sSearch=&bRegex=false&sSearch_0=&bRegex_0=false&bSearchable_0=true&sSearch_1=&bRegex_1=false&bSearchable_1=true&sSearch_2=&bRegex_2=false&bSearchable_2=true&sSearch_3=&bRegex_3=false&bSearchable_3=true&sSearch_4=&bRegex_4=false&bSearchable_4=true&iSortCol_0=1&sSortDir_0=asc&iSortingCols=1&bSortable_0=true&bSortable_1=true&bSortable_2=true&bSortable_3=true&bSortable_4=true";

const std::string sampleResp = "HTTP/1.1 200 OK\r\n\
Content-Type: text/xml; charset=utf-8\r\n\
Content-Length: 200\r\n\r\n\
<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\
<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n\
  <soap:Body>\n\
  <EnlightenResponse xmlns=\"http://clearforest.com/\">\n\
  <EnlightenResult>string</EnlightenResult>\n\
  </EnlightenResponse>\n\
  </soap:Body>\n\
</soap:Envelope>";


int main()
{      
    Skit::HTTP::Request req;
    Skit::HTTP::Request reqBuild;
    Skit::HTTP::Response respBuild;
    Skit::HTTP::Response response;
    Log::ReportingLevel() = logDEBUG;
 
    req.Init(postreq);
    std::cout << "method " << req.GetMethod() << std::endl;
    std::cout << "protocol " <<req.GetProtocol() << std::endl;
    std::cout << "URL " <<req.GetURL() << std::endl;
    std::cout << "URI " <<req.GetURI() << std::endl;
    
    std::cout << "--------------------------------------\n";
    
    req.Reset();
    req.Init(getreqparams);
    std::cout << "method " << req.GetMethod() << std::endl;
    std::cout << "protocol " <<req.GetProtocol() << std::endl;
    std::cout << "URL " <<req.GetURL() << std::endl;
    std::cout << "URI " <<req.GetURI() << std::endl;
    
    std::cout << "--------------------------------------\n";
    
    req.Reset();
    req.Init(postreqparams);
    std::cout << "method " << req.GetMethod() << std::endl;
    std::cout << "protocol " <<req.GetProtocol() << std::endl;
    std::cout << "URL " <<req.GetURL() << std::endl;
    std::cout << "URI " <<req.GetURI() << std::endl;

    std::cout << "--------------------------------------\n";
    
    req.Reset();
    //req.Init(postReqParams1);
    req.Init(postreqparams1);
    std::cout << "method " << req.GetMethod() << std::endl;
    std::cout << "protocol " <<req.GetProtocol() << std::endl;
    std::cout << "URL " <<req.GetURL() << std::endl;
    std::cout << "URI " <<req.GetURI() << std::endl;
    
    std::cout << "--------------------------------------\n";
    
    response.Init(sampleResp);
    std::cout << "Response code " << response.GetResponseCode() << std::endl;
    std::cout << "Response message " << response.GetResponseMessage() << std::endl;
    std::cout << "Body " << response.GetBody() << std::endl;
    
    std::cout << "--------------------------------------\n";
    
    reqBuild.Reset();
    reqBuild.SetHeader("Host", "api.opencalais.com");
    reqBuild.SetHeader("Content-Type", "text/xml; charset=utf-8");
    reqBuild.SetHeader("SOAPAction", "\"http://clearforest.com/Enlighten\"");
    std::string& outRequest = reqBuild.BuildRequest("POST", "/enlighten/calais.asmx");
    std::cout << outRequest;
    
    std::cout << "--------------------------------------\n";

    reqBuild.Reset();
    reqBuild.SetHeader("Host", "api.opencalais.com");
    reqBuild.SetHeader("Content-Type", "application/x-www-form-urlencoded");
    outRequest = reqBuild.BuildRequest("GET", "/enlighten/calais.asmx/Enlighten?licenseID=string&content=string&paramsXML=string");
    std::cout << outRequest;
    
    std::cout << "--------------------------------------\n";

    reqBuild.Reset();
    reqBuild.SetHeader("Host", "api.opencalais.com");
    reqBuild.SetHeader("Content-Type", "application/x-www-form-urlencoded");
    reqBuild.SetParamVar("licenseID", "435rfcwe");
    reqBuild.SetParamVar("content", "554ghhsaaF4");
    reqBuild.SetParamVar("paramsXML", "2345fcjhq2");
    outRequest = reqBuild.BuildRequest("GET", "/enlighten/calais.asmx/Enlighten");
    std::cout << outRequest;
    
    std::cout << "method " << req.GetMethod() << std::endl;
    std::cout << "protocol " <<req.GetProtocol() << std::endl;
    std::cout << "URL " <<req.GetURL() << std::endl;
    std::cout << "URI " <<req.GetURI() << std::endl;
    std::cout << reqBuild.GetParamVar("licenseID") << std::endl;
    std::cout << reqBuild.GetParamVar("content") << std::endl; 
    std::cout << reqBuild.GetParamVar("paramsXML") << std::endl;
    
    std::cout << "--------------------------------------\n";
    
    reqBuild.Reset();
    reqBuild.SetHeader("Host", "api.opencalais.com");
    reqBuild.SetHeader("Content-Type", "application/x-www-form-urlencoded");
    reqBuild.SetParamVar("licenseID", "435rfcwe");
    reqBuild.SetParamVar("content", "554ghhsaaF4");
    reqBuild.SetParamVar("paramsXML", "2345fcjhq2");
    outRequest = reqBuild.BuildRequest("POST", "/enlighten/calais.asmx/Enlighten");
    std::cout << outRequest << std::endl;
    
    std::cout << "--------------------------------------\n";
    
    respBuild.Reset();
    respBuild.SetHeader("Content-Type", "text/xml; charset=utf-8");
    const char * body = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\
<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n\
  <soap:Body>\n\
  <EnlightenResponse xmlns=\"http://clearforest.com/\">\n\
  <EnlightenResult>string</EnlightenResult>\n\
  </EnlightenResponse>\n\
  </soap:Body>\n\
</soap:Envelope>";
    respBuild.SetBody(body);
    std::string outResponse = respBuild.BuildResponse("200", "OK");
    std::cout << outResponse << std::endl;
    

    return 0;
}





