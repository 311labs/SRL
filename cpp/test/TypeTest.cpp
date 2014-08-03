
#include "TypeTest.h"

#include "srl/DateTime.h"
#include "srl/Timer.h"

using namespace SRL;
using namespace SRL::Test;

bool TypeTest::sizeTest()
{
	// check size of each data type
	TEST_ASSERT(sizeof(int32)==4)
	TEST_ASSERT(sizeof(int64)==8)
	TEST_ASSERT(sizeof(uint32)==4)
	TEST_ASSERT(sizeof(uint64)==8)
	TEST_ASSERT(sizeof(float32)==4)
	TEST_ASSERT(sizeof(float64)==8)
	TEST_ASSERT(sizeof(byte)==1)
	TEST_ASSERT(sizeof(Bool)==1)
	TEST_ASSERT(sizeof(char)==1)
	return true;
}

bool TypeTest::dateTest()
{
	// construct a date
	Date dt;
	bool flag=false;
	if ((dt.year() > 2003)&&(dt.year() < 2080)&&
		(dt.month() > 0) && (dt.month() <= 12)&&
		(dt.day() > 0) && (dt.day() <= 31))
	{
		flag = true;
	}
	else
	{
        return false;
	}	


	Date dt2(2, 5, 1975);
	TEST_ASSERT((dt2.day()==2) && (dt2.month()==5) && (dt2.year() == 1975))
	
	// test if day of week works
	TEST_ASSERT(dt2.dayOfWeek() == 5);
	TEST_ASSERT(dt2.dayOfYear() == 122);
	TEST_ASSERT(dt2.daysInMonth() == 31);
	TEST_ASSERT(!dt2.isLeapYear());
	dt2.addToYear(40);
	TEST_ASSERT((dt2.day()==2) && (dt2.month()==5) && (dt2.year() == 1975+40));
	dt2.addToMonth(-1*(40*12));
	TEST_ASSERT((dt2.day()==2) && (dt2.month()==5) && (dt2.year() == 1975));
	dt2.addToMonth(40*12);
	TEST_ASSERT((dt2.day()==2) && (dt2.month()==5) && (dt2.year() == 1975+40));
	dt2.addToYear(-40);
	dt2.addToDay(300);
	TEST_ASSERT(dt2.year() == 1976);
	dt2 -= 300;
	TEST_ASSERT((dt2.day()==2) && (dt2.month()==5) && (dt2.year() == 1975));
	Date dt3 = dt2 + 300;
	TEST_ASSERT(dt3.year() == 1976);
	Date diff = dt3 - dt2;
	TEST_ASSERT(diff.julian()==300);
	//Date dt3("May 2, 1975");
	//TEST_ASSERT((dt2.day()==2) && (dt2.month()==5) && (d2t.year() == 1975))

	//Date dt3("1975/5/2");
	//TEST_ASSERT((dt2.day()==2) && (dt2.month()==5) && (dt2.year() == 1975))
	dt2.update();
	TEST_ASSERT((dt.day()==dt2.day())&&(dt.month()==dt2.month())&&(dt.year()&&dt2.year()));
	TEST_ASSERT(dt.isToday());
	TEST_ASSERT(dt2 == dt);
	TEST_ASSERT(dt2 >= dt);
	TEST_ASSERT(dt2 <= dt);
	dt2.addToYear(2);
	TEST_ASSERT(dt2 > dt);
	TEST_ASSERT(dt < dt2);
	TEST_ASSERT(dt != dt2);
	return true;
}

bool TypeTest::timeTest()
{
	SRL::Time t;
	TEST_ASSERT(t.hour() < 24 && t.minute() < 60 && t.second() < 60);
	// string construction
	//Time ts("14:30:26");
	SRL::Time th(5, 2, 58);
	TEST_ASSERT(th.hour() == 5 && th.minute() == 2 && th.second() == 58);
	TEST_ASSERT(th.getTime() == 18178);

	th.update();
    printf("time: %0.2f\n", th.getTime());
	t = th;
	
	TEST_ASSERT(t == th);
	TEST_ASSERT(t <= th);
	TEST_ASSERT(t >= th);
	th += 75;
	TEST_ASSERT(t != th);
	TEST_ASSERT(t < th);
	TEST_ASSERT(th > t);
	return true;
}

bool TypeTest::timerTest()
{
	Timer timer;
	
	timer.start();
	System::Sleep(500);
	timer.stop();
	TEST_ASSERT((timer.totalMS() > 490) && (timer.totalMS() < 520));
	return true;
}

bool TypeTest::dtTest()
{
	DateTime dt;
	TEST_ASSERT(dt.year() < 2050 && dt.month() <= 12 && dt.day() <= 31 && dt.hour() < 24 && dt.minute() < 60 && dt.second() < 60);
	DateTime dt2(2, 5, 1975, 1, 35, 5);
	TEST_ASSERT(dt2.year() == 1975 && dt2.month() == Date::MAY && dt2.day() == 2 && dt2.hour() == 1 && dt2.minute() == 35 && dt2.second() == 5);
	dt2.addToHour(23);
	TEST_ASSERT(dt2.day() == 3);
	TEST_ASSERT(dt2.hour() == 0);
	TEST_ASSERT(dt2.minute() == 35);
	TEST_ASSERT(dt2.year() == 1975 && dt2.month() == Date::MAY);
	dt2.addToHour(7200);
	TEST_ASSERT(dt2.year() == 1976);
	TEST_ASSERT(dt2.day() != 2);
	TEST_ASSERT(dt2.minute() == 35);
	TEST_ASSERT(dt2.hour() == 0);
	return true;

}
