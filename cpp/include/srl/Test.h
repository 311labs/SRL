#ifndef SRL_ASSERT
#define SRL_ASSERT

#include "srl/Console.h"
#include "srl/Exceptions.h" 
#include "srl/sys/System.h"

#define HTML_HEADER "<html> \
    <head>\
        <style>\
            body { background-color: lightgreen; color: navy; font: Arial; } \
            .title { font-size: 24pt; color: white; text-shadow: 3px 3px 5px black; }\
            .test_box { margin: 15px; border: 2px solid blue; background-color: white; } \
            .test_title { text-shadow: 2px 2px 1px black; border-bottom: 1px solid black; font-size: 14pt; margin-top: 10px; }\
            .test_name { text-align: right; padding-right: 5px; font-weight: bold; } \
            .test_success { color: green; }\
            .test_failed { color: red; font-weight: bold; }\
            .test_failed_reason { background-color: #FFCDD3; color: red; border: 1px dashed darkred; border-left: 5px solid darkred; margin: 10px }\
        .end_success { background-color: lightyellow; color: #336633; border: 5px solid #66CC66; }\
        .end_error { background-color: #FFCDD3; color: darkred; border: 5px solid darkred;}\
        .end_success, end_error { font-size: 18pt; text-shadow: 2px 2px 2px black; margin-top: 10px; margin-left: 5px; }\
        </style>\
    </head>\
    <body>\
"

#define SCROLL_TO_HERE "<script language=\"JavaScript1.2\">window.scroll(0,document.documentElement.clientHeight);</script>"
 
namespace SRL
{ 
    namespace Test
    {   
        class UnitTestManager;
            
        class UnitTest
        {
        friend class UnitTestManager;
        public:
         UnitTest(const char* title, bool html=false):_total(0),_failed(0), _html(html), _title(title){}
         virtual ~UnitTest(){}
         virtual void run_tests(){}
         void run()
         {
             printTitle(_title);
             run_tests();
             printResults();             
         };
 
         int getTestCount() const { return _total;}
         int getFailedCount() const { return _failed;}
        protected:
            int _total, _failed;
            bool _html;
            SRL::String _title;
    
            void printTitle(const String& title)
            {
                if (_html)
                {
                    SRL::Console::writeLine("<div class=\"test_box\">");
                    SRL::Console::formatLine("<div class=\"test_title\">%s</div>", title.text());
                    Console::writeLine("<table border=0 width='100%'>");     
                }
                else
                {
                    SRL::Console::writeLine("=================================");
                    SRL::Console::write(title);
                    SRL::Console::writeLine(" Tests");
                    SRL::Console::writeLine("=================================");                    
                }
            }
            
            void printTestName(const char* name)
            {
                if (_html)
                {
                    Console::formatLine("<tr><td width=\"180px\" class=\"test_name\">%s</td>", name);
                }
                else
                {
                    SRL::Console::format("%25s: ", name); 
                }              
            }
            
            void printSuccess()
            {
                if (_html)
                {
                    Console::writeLine("<td class=\"test_success\">passed</td></tr>");
                    Console::writeLine(SCROLL_TO_HERE);
                }
                else
                {
                    SRL::Console::writeLine("passed");
                }                
            }

            void printFailed(const char* exp, const char* filename, int filenumber)
            {
                if (_html)
                {
                    Console::writeLine("<td class=\"test_failed\">failed</td></tr>");
                    Console::write("<tr><td colspan=2 class=\"test_failed_reason\">");
                    Console::formatLine("<a href=\"txmt://open?url=file:///Users/ians/Projects/srl/%s&line=%d&column=0\">'", filename, filenumber);
                    Console::write(exp);
                    Console::formatLine("'<br />%s:%d", filename, filenumber);
                    Console::writeLine("</a>");
                    Console::writeLine("</td></tr>");

                }
                else
                {
                    SRL::Console::writeLine("FAILED");
                    SRL::Console::formatLine("\t\t\t'%s'", exp); \
                    SRL::Console::formatLine("\t\t\tline %d in %s\n", filenumber, filename); \
                }
                ++_failed;             
            }
            
            void printExeception(const SRL::Errors::Exception &e)
            {
                if (_html)
                {
                    Console::writeLine("<td class=\"test_failed\">failed</td></tr>");
                    Console::write("<div class=\"test_failed_reason\"><span class=\"test_failed_reason\">Exception: ");
                    Console::write(e.message());
                    Console::writeLine("</td></tr>");
                }
                else
                {
                    SRL::Console::write("FAILED\n\t\t\t");
                    SRL::Console::writeLine(e.message());                   
                }
                ++_failed;                  
            }
            
            void printResults()
            {
                if (_html)
                {
                    Console::writeLine("</table></div>");
                }
                else
                {
                    SRL::Console::writeLine("==================================");                    
                    if (_failed == 0)
                        SRL::Console::formatLine("All %d Tests Completed Succesfully!\n", _total);
                    else
                        SRL::Console::formatLine("%d of %d %s Tests FAILED!\n", _failed, _total, _title.text());                    
                }
            }
            
        };

        class UnitTestManager
        {
        public:
            UnitTestManager(bool html=false) : _failed(0), _test_count(0), _html(html)
            {
                
            }
            
            virtual ~UnitTestManager (){}
            
            void add(UnitTest *test)
            {
                test->_html = _html;
                _tests[_test_count] = test;
                ++_test_count;
            }
            
            void listTests()
            {
                for( uint32 i = 0; i < _test_count; ++i )
                {
                    Console::writeLine(_tests[i]->_title);
                }               
            }
            
            UnitTest* getTest(const String& name)
            {
                for(uint32 i = 0; i < _test_count; ++i)
                {
                    if (_tests[i]->_title == name)
                        return _tests[i];
                }
                return NULL;
            }
            
            void run(String names[], uint32 len)
            {
                //bool bombed = false;
                printHeader();
                
                for(uint32 i = 0; i < len; ++i)
                {
                    UnitTest* test = getTest(names[i]);
                    if (test != NULL)
                    {
                        test->run();
                        _failed += test->getFailedCount();
                    }
                }
                
                printFooter();                
            }
            
            void printHeader()
            {
                if (_html)
                {
                    Console::writeLine(HTML_HEADER);
                    SRL::Console::writeLine("<div class=\"title\">SRL Unit Tests</div>");
                }                
                else
                {
                    
                }
            }
            
             
            bool run()
            {          
                bool bombed = false;
                printHeader();
                
                try
                {
                    for( uint32 i = 0; i < _test_count; ++i )
                    {
                        _tests[i]->run();
                        _failed += _tests[i]->getFailedCount();
                    }
                }
                catch(Errors::Exception &e)
                {
                    _last_error = e.message();
                    ++_failed;
                    bombed = true;
                }
                catch(...)
                {
                    _last_error = System::GetLastErrorString();
                    bombed = true;
                    ++_failed;
                }
                
                printFooter();
                return bombed;
            }
            
            void printFooter()
            {    
                if (_html)
                {
                    if (_failed > 0)
                    {
                        Console::write("<div class=\"end_error\">");
                        if (_last_error.length() > 2)
                            SRL::Console::formatLine("%s<br />", _last_error.text());
                        SRL::Console::formatLine("%d TESTS FAILED!", _failed);
                    }
                    else
                    {
                        Console::write("<div class=\"end_success\">");
                        SRL::Console::writeLine("all tests passed");
                    }
                    Console::writeLine("</div>");
                    Console::writeLine(SCROLL_TO_HERE);                 
                    Console::writeLine("</body></html>");
                }                
                else
                {
                    SRL::Console::writeLine("=================================");
                    if (_failed > 0)
                    {
                        if (_last_error.length() > 2)
                            SRL::Console::writeLine(_last_error);
                        SRL::Console::formatLine("%d TESTS FAILED!", _failed);
                    }
                    else
                        SRL::Console::writeLine("all tests passed");
                    SRL::Console::writeLine("=================================");
                }
            }
            
            
            
        private:
            uint32 _failed;
            UnitTest *_tests[50];
            uint32 _test_count;
            bool _html;
            String _last_error;
        };
        
        
     }
 }
  
  
  #define RUN_TEST(__name, __func) \
        printTestName(__name); ++_total; \
        try { if (__func) \
             printSuccess(); \
        } catch(SRL::Errors::Exception &e){ \
            printExeception(e); \
        } 

/* standard assert macro */
  #define TEST_ASSERT(__exp) \
        if (!(__exp)) { \
            printFailed(#__exp, __FILE__, __LINE__); \
            return false; \
        } \
        
/* an assert variant that prints out a user supplied message  */
  #define TEST_ASSERT_MSG(__exp, __msg) \
        if (!(__exp)) { \
            printFailed(__msg, __FILE__, __LINE__); \
            return false; \
        } \

  #define TEST_THROWS(__exp, __exc) \
		{ bool __expected = false; \
		try { __exp; } \
		catch (__exc) { __expected = true; } \
		catch (...){} \
		if (!__expected) { \
		    String tmp = String::Format("'%s' did not throw '%s'",  #__exp, #__exc); \
		    printFailed(tmp.text(), __FILE__, __LINE__); \
            return false;\
        } }

    #define TEST_THROWS_NOTHING(__exp)									\
		{ bool __expected = true;										\
		try { __exp; } 												\
		catch (...) { __expected = false; }							\
		if (!__expected)											\
		{ \
		    String tmp = String::Format("'%s' exception thrown '%d'",  #__exp, __LINE__); \
		    printFailed(tmp.text(), __FILE__, __LINE__); \
            return false;\
		} }

#endif

