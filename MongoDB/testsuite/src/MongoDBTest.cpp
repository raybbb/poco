//
// MongoDBTest.cpp
//
// $Id$
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
#include <iostream>

#include "Poco/DateTime.h"
#include "Poco/ObjectPool.h"

#include "Poco/MongoDB/InsertRequest.h"
#include "Poco/MongoDB/QueryRequest.h"
#include "Poco/MongoDB/DeleteRequest.h"
#include "Poco/MongoDB/GetMoreRequest.h"
#include "Poco/MongoDB/PoolableConnectionFactory.h"
#include "Poco/MongoDB/Database.h"
#include "Poco/MongoDB/Cursor.h"

#include "Poco/Net/NetException.h"

#include "MongoDBTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"

using namespace Poco::MongoDB;


MongoDBTest::MongoDBTest(const std::string& name)
	: CppUnit::TestCase("MongoDB")
	, _connected(false)
{
}


MongoDBTest::~MongoDBTest()
{
}


void MongoDBTest::setUp()
{
	try
	{
		_mongo.connect("localhost", 27017);
		_connected = true;
	}
	catch(Poco::Net::ConnectionRefusedException& e)
	{
		std::cout << "Couldn't connect to " << e.message() << ". ";
	}
}


void MongoDBTest::tearDown()
{
	if ( _connected )
	{
		_mongo.disconnect();
	}
}


void MongoDBTest::testInsertRequest()
{
	if ( ! _connected )
	{
		std::cout << "test skipped." << std::endl;
		return;
	}

	Poco::MongoDB::Document::Ptr player = new Poco::MongoDB::Document();
	player->add("lastname", std::string("Braem"));
	player->add("firstname", std::string("Franky"));

	Poco::DateTime birthdate;
	birthdate.assign(1969, 3, 9);
	player->add("birthdate", birthdate.timestamp());

	player->add("start", 1993);
	player->add("active", false);

	Poco::DateTime now;
	std::cout << now.day() << " " << now.hour() << ":" << now.minute() << ":" << now.second() << std::endl;
	player->add("lastupdated", now.timestamp());

	player->add("unknown", NullValue());

	Poco::MongoDB::InsertRequest request("team.players");
	request.documents().push_back(player);
	_mongo.sendRequest(request);
}

void MongoDBTest::testQueryRequest()
{
	if ( ! _connected )
	{
		std::cout << "test skipped." << std::endl;
		return;
	}

	Poco::MongoDB::QueryRequest request("team.players");
	request.query().add("lastname" , std::string("Braem"));
	request.setNumberToReturn(1);

	Poco::MongoDB::ResponseMessage response;

	_mongo.sendRequest(request, response);

	if ( response.documents().size() > 0 )
	{
		Poco::MongoDB::Document::Ptr doc = response.documents()[0];

		try
		{
			std::string lastname = doc->get<std::string>("lastname");
			assert(lastname.compare("Braem") == 0);
			std::string firstname = doc->get<std::string>("firstname");
			assert(firstname.compare("Franky") == 0);
			Poco::Timestamp birthDateTimestamp = doc->get<Poco::Timestamp>("birthdate");
			Poco::DateTime birthDate(birthDateTimestamp);
			assert(birthDate.year() == 1969 && birthDate.month() == 3 && birthDate.day() == 9);
			Poco::Timestamp lastupdatedTimestamp = doc->get<Poco::Timestamp>("lastupdated");
			assert(doc->isType<NullValue>("unknown"));
			bool active = doc->get<bool>("active");
			assert(!active);

			std::string id = doc->get("_id")->toString();
			std::cout << id << std::endl;
		}
		catch(Poco::NotFoundException& nfe)
		{
			fail(nfe.message() + " not found.");
		}
	}
	else
	{
		fail("No document returned");
	}
}

void MongoDBTest::testDBQueryRequest()
{
	if ( ! _connected )
	{
		std::cout << "test skipped." << std::endl;
		return;
	}

	Database db("team");
	Poco::SharedPtr<Poco::MongoDB::QueryRequest> request = db.createQueryRequest("players");
	request->query().add("lastname" , std::string("Braem"));

	Poco::MongoDB::ResponseMessage response;
	_mongo.sendRequest(*request, response);

	if ( response.documents().size() > 0 )
	{
		Poco::MongoDB::Document::Ptr doc = response.documents()[0];

		try
		{
			std::string lastname = doc->get<std::string>("lastname");
			assert(lastname.compare("Braem") == 0);
			std::string firstname = doc->get<std::string>("firstname");
			assert(firstname.compare("Franky") == 0);
			Poco::Timestamp birthDateTimestamp = doc->get<Poco::Timestamp>("birthdate");
			Poco::DateTime birthDate(birthDateTimestamp);
			assert(birthDate.year() == 1969 && birthDate.month() == 3 && birthDate.day() == 9);
			Poco::Timestamp lastupdatedTimestamp = doc->get<Poco::Timestamp>("lastupdated");
			assert(doc->isType<NullValue>("unknown"));

			std::string id = doc->get("_id")->toString();
			std::cout << id << std::endl;
		}
		catch(Poco::NotFoundException& nfe)
		{
			fail(nfe.message() + " not found.");
		}
	}
	else
	{
		fail("No document returned");
	}
}


void MongoDBTest::testCountCommand()
{
	if ( ! _connected )
	{
		std::cout << "test skipped." << std::endl;
		return;
	}

	Poco::MongoDB::QueryRequest request("team.$cmd");
	request.setNumberToReturn(1);
	request.query().add("count", std::string("players"));

	Poco::MongoDB::ResponseMessage response;

	_mongo.sendRequest(request, response);

	if ( response.documents().size() > 0 )
	{
		Poco::MongoDB::Document::Ptr doc = response.documents()[0];
		double count = doc->get<double>("n");
		assert(count == 1);
	}
	else
	{
		fail("Didn't get a response from the count command");
	}
}


void MongoDBTest::testDBCountCommand()
{
	if ( ! _connected )
	{
		std::cout << "test skipped." << std::endl;
		return;
	}

	Poco::MongoDB::Database db("team");
	Poco::SharedPtr<Poco::MongoDB::QueryRequest> request = db.createCountRequest("players");

	Poco::MongoDB::ResponseMessage response;
	_mongo.sendRequest(*request, response);

	if ( response.documents().size() > 0 )
	{
		Poco::MongoDB::Document::Ptr doc = response.documents()[0];
		double count = doc->get<double>("n");
		assert(count == 1);
	}
	else
	{
		fail("Didn't get a response from the count command");
	}
}


void MongoDBTest::testDBCount2Command()
{
	if ( ! _connected )
	{
		std::cout << "test skipped." << std::endl;
		return;
	}

	Poco::MongoDB::Database db("team");
	double count = db.count(_mongo, "players");
	assert(count == 1);
}


void MongoDBTest::testDeleteRequest()
{
	if ( ! _connected )
	{
		std::cout << "test skipped." << std::endl;
		return;
	}

	Poco::MongoDB::DeleteRequest request("team.players");
	request.selector().add("lastname", std::string("Braem"));

	_mongo.sendRequest(request);
}


void MongoDBTest::testCursorRequest()
{
	if ( ! _connected )
	{
		std::cout << "test skipped." << std::endl;
		return;
	}

	Poco::MongoDB::Database db("team");
	Poco::SharedPtr<Poco::MongoDB::InsertRequest> insertRequest = db.createInsertRequest("numbers");
	for(int i = 0; i < 10000; ++i)
	{
		Document::Ptr doc = new Document();
		doc->add("number", i);
		insertRequest->documents().push_back(doc);
	}
	_mongo.sendRequest(*insertRequest);

	double count = db.count(_mongo, "numbers");
	assert(count == 10000);

	Poco::MongoDB::Cursor cursor("team", "numbers");

	int n = 0;
	Poco::MongoDB::ResponseMessage& response = cursor.next(_mongo);
	while(response.cursorID() != 0)
	{
		n += response.documents().size();
		response = cursor.next(_mongo);
	}
	std::cout << "n= " << n << std::endl;
	assert(n == 10000);

	Poco::MongoDB::QueryRequest drop("team.$cmd");
	drop.setNumberToReturn(1);
	drop.query().add("drop", std::string("numbers"));

	Poco::MongoDB::ResponseMessage responseDrop;
	_mongo.sendRequest(drop, responseDrop);

	if ( responseDrop.documents().size() > 0 )
	{
		std::cout << responseDrop.documents()[0]->toString(2) << std::endl;
	}
}


void MongoDBTest::testBuildInfo()
{
	if ( ! _connected )
	{
		std::cout << "test skipped." << std::endl;
		return;
	}

	Poco::MongoDB::QueryRequest request("team.$cmd");
	request.setNumberToReturn(1);
	request.query().add("buildInfo", 1);

	Poco::MongoDB::ResponseMessage response;

	try
	{
		_mongo.sendRequest(request, response);
	}
	catch(Poco::NotImplementedException& nie)
	{
		std::cout << nie.message() << std::endl;
		return;
	}

	if ( response.documents().size() > 0 )
	{
		Poco::MongoDB::Document::Ptr doc = response.documents()[0];
		std::cout << doc->toString();
	}
	else
	{
		fail("Didn't get a response from the buildinfo command");
	}
}


void MongoDBTest::testConnectionPool()
{
	Poco::PoolableObjectFactory<Poco::MongoDB::Connection, Poco::MongoDB::Connection::Ptr> factory("localhost:27017");
	Poco::ObjectPool<Poco::MongoDB::Connection, Poco::MongoDB::Connection::Ptr> pool(factory, 10, 15);

	Poco::MongoDB::PooledConnection pooledConnection(pool);

	Poco::MongoDB::QueryRequest request("team.$cmd");
	request.setNumberToReturn(1);
	request.query().add("count", std::string("players"));

	Poco::MongoDB::ResponseMessage response;
	((Connection::Ptr) pooledConnection)->sendRequest(request, response);

	if ( response.documents().size() > 0 )
	{
		Poco::MongoDB::Document::Ptr doc = response.documents()[0];
		double count = doc->get<double>("n");
		assert(count == 1);
	}
	else
	{
		fail("Didn't get a response from the count command");
	}
}

CppUnit::Test* MongoDBTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("MongoDBTest");

	CppUnit_addTest(pSuite, MongoDBTest, testInsertRequest);
	CppUnit_addTest(pSuite, MongoDBTest, testQueryRequest);
	CppUnit_addTest(pSuite, MongoDBTest, testDBQueryRequest);
	CppUnit_addTest(pSuite, MongoDBTest, testCountCommand);
	CppUnit_addTest(pSuite, MongoDBTest, testDBCountCommand);
	CppUnit_addTest(pSuite, MongoDBTest, testDBCount2Command);
	CppUnit_addTest(pSuite, MongoDBTest, testConnectionPool);
	CppUnit_addTest(pSuite, MongoDBTest, testDeleteRequest);
	CppUnit_addTest(pSuite, MongoDBTest, testBuildInfo);
	CppUnit_addTest(pSuite, MongoDBTest, testCursorRequest);

	return pSuite;
}
