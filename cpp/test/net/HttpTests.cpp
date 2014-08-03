#include "HttpTests.h"

#include "srl/net/URI.h"

using namespace SRL;
using namespace SRL::Net;
using namespace SRL::Test;


bool HttpTest::uriTest()
{
    URI uri("http://srl.starnes.de/wiki/BasicTypes");
    TEST_ASSERT(uri.scheme() == "http");
    TEST_ASSERT(uri.user().isEmpty());
    TEST_ASSERT(uri.host() == "srl.starnes.de");
    TEST_ASSERT(uri.port() == 0);
    TEST_ASSERT(uri.path() == "/wiki/BasicTypes");
    TEST_ASSERT(uri.query().isEmpty());
    TEST_ASSERT(uri.fragment().isEmpty());
    TEST_ASSERT(uri.asString() == "http://srl.starnes.de/wiki/BasicTypes");
    
    uri.set("svn+ssh://ians@srl.starnes.de/var/svnreps/srl/trunk");
    TEST_ASSERT(uri.scheme() == "svn+ssh");
    TEST_ASSERT(uri.user() == "ians");
    TEST_ASSERT(uri.host() == "srl.starnes.de");
    TEST_ASSERT(uri.port() == 0);
    TEST_ASSERT(uri.path() == "/var/svnreps/srl/trunk");
    TEST_ASSERT(uri.query().isEmpty());
    TEST_ASSERT(uri.fragment().isEmpty());
    TEST_ASSERT(uri.asString() == "svn+ssh://ians@srl.starnes.de/var/svnreps/srl/trunk");
    
    uri.set("https://ians@www.google.com:8080/projects/ximian?password=monkey&food=banana");
    TEST_ASSERT(uri.scheme() == "https");
    TEST_ASSERT(uri.user() == "ians");
    TEST_ASSERT(uri.host() == "www.google.com");
    TEST_ASSERT(uri.port() == 8080);
    TEST_ASSERT(uri.path() == "/projects/ximian");
    TEST_ASSERT(uri.query() == "password=monkey&food=banana");
    TEST_ASSERT(uri.fragment().isEmpty());
    TEST_ASSERT(uri.asString() == "https://ians@www.google.com:8080/projects/ximian?password=monkey&food=banana");    
    
    uri.set("https://ians:FAAMVAE@srl.starnes.de/projects/ximian.py#banana");
    TEST_ASSERT(uri.scheme() == "https");
    TEST_ASSERT(uri.user() == "ians:FAAMVAE");
    TEST_ASSERT(uri.host() == "srl.starnes.de");
    TEST_ASSERT(uri.port() == 0);
    TEST_ASSERT(uri.path() == "/projects/ximian.py");
    TEST_ASSERT(uri.query().isEmpty());
    TEST_ASSERT(uri.fragment() == "banana");
    TEST_ASSERT(uri.asString() == "https://ians:FAAMVAE@srl.starnes.de/projects/ximian.py#banana");    

    // UN ENCODED URI's
    uri.set("https://srl.starnes.de/projects/ian beard/cgi.py?pictures='35'&space=to moon&poney=a!", false);
    TEST_ASSERT(uri.scheme() == "https");
    TEST_ASSERT(uri.user().isEmpty());
    TEST_ASSERT(uri.host() == "srl.starnes.de");
    TEST_ASSERT(uri.port() == 0);
    TEST_ASSERT(uri.path() == "/projects/ian%20beard/cgi.py");
    TEST_ASSERT(uri.query() == "pictures=%2735%27&space=to+moon&poney=a%21");
    TEST_ASSERT(uri.fragment().isEmpty());
    TEST_ASSERT(uri.asString() == "https://srl.starnes.de/projects/ian%20beard/cgi.py?pictures=%2735%27&space=to+moon&poney=a%21");
    String decoded(80);
    URI::Decode(uri.asString(), decoded);
    TEST_ASSERT(decoded == "https://srl.starnes.de/projects/ian beard/cgi.py?pictures='35'&space=to moon&poney=a!");
    
    // now lets test adding fields
    uri.addQueryField("pictures", "'55'");
    uri.addQueryField("world", "Hell:is a question?");    
    TEST_ASSERT(uri.asString() == "https://srl.starnes.de/projects/ian%20beard/cgi.py?pictures=%2735%27&pictures=%2755%27&space=to+moon&poney=a%21&world=Hell%3Ais+a+question%3F");
    
    
    
    
    
    return true;
}