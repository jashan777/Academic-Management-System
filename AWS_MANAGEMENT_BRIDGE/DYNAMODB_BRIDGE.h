#pragma once

#include "AWS_MANAGEMENT_BRIDGE.h"

#include "../AWS_MANAGEMENT_BASE/core.h"

using namespace System;
using namespace System::Collections::Generic;

namespace AWS_MANAGEMENT_BRIDGE
{
	public ref class DYNAMODB_BRIDGE : public ManagedObject<base::dynamodb_base>
	{
	public:
		DYNAMODB_BRIDGE();
		int CreateDynamoDBConnection();
		array<String^>^ ListTables();
		Dictionary<String^,String^>^ GetItems(String^ tablename, String^ partitionkey, String^ partitionvalue);
		void CloseConnection();
	};
}