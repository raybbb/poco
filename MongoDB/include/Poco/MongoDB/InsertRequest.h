//
// InsertRequest.h
//
// $Id$
//
// Library: MongoDB
// Package: MongoDB
// Module:  InsertRequest
//
// Definition of the InsertRequest class.
//
// Copyright (c) 2012, Applied Informatics Software Engineering GmbH.
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

#ifndef _MongoDB_InsertRequest_included
#define _MongoDB_InsertRequest_included

#include "Poco/MongoDB/MongoDB.h"
#include "Poco/MongoDB/RequestMessage.h"
#include "Poco/MongoDB/Document.h"

namespace Poco {
namespace MongoDB {


class MongoDB_API InsertRequest : public RequestMessage
	/// Class for creating an OP_INSERT client request. This request is used
	/// to insert one or more documents to the database.
{
public:
	typedef enum
	{
		INSERT_NONE = 0,
		INSERT_CONTINUE_ON_ERROR = 1
	} Flags;


	InsertRequest(const std::string& collectionName, Flags flags = INSERT_NONE );
		/// Constructor.
		/// The full collection name is the concatenation of the database 
		/// name with the collection name, using a "." for the concatenation. For example, 
		/// for the database "foo" and the collection "bar", the full collection name is 
		/// "foo.bar".

	virtual ~InsertRequest();
		/// Destructor


	Document::Vector& documents();
		/// Returns the documents to insert into the database

protected:

	void buildRequest(BinaryWriter& writer);


private:

	Int32 _flags;

	std::string _fullCollectionName;

	Document::Vector _documents;
};


inline Document::Vector& InsertRequest::documents()
{
	return _documents;
}


}} // Namespace Poco::MongoDB

#endif //_MongoDB_InsertRequest_included
