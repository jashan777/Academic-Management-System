//DynamoDb base headerFile
#include "dynamodb_base.h"
#include <string>
#include <vector>
#include <map>

//DYNAMO DB HEADER FILES.
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/ListTablesRequest.h>
#include <aws/dynamodb/model/ListTablesResult.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/GetItemRequest.h>
//CREATING HEADER FILES FOR CREATING TABLE.
#include <aws/dynamodb/model/CreateTableRequest.h>
#include <aws/dynamodb/model/KeySchemaElement.h>
#include <aws/dynamodb/model/ProvisionedThroughput.h>
#include <aws/dynamodb/model/ScalarAttributeType.h>

namespace base
{

    std::shared_ptr<Aws::DynamoDB::DynamoDBClient> DynamoDB_clientRef;

    int dynamodb_base::CreateDynamoDBConnection()
    {
        Aws::Client::ClientConfiguration config;
        config.region = Aws::Region::AP_SOUTH_1;
        try
        {
            auto DynamoDB_client = Aws::MakeShared<Aws::DynamoDB::DynamoDBClient>("dynamoDBclient", config);
            DynamoDB_clientRef = DynamoDB_client;
        }
        catch (...)
        {
            return 0;
        }
        return 1;
    }

    dynamodb_base::~dynamodb_base()
    {
        DynamoDB_clientRef = nullptr;
    }

    std::string AWSDynamoDbResultParser(Aws::DynamoDB::Model::ValueType obj, Aws::DynamoDB::Model::AttributeValue val)
    {
        //type of enums NUMBER, BYTEBUFFER, STRING_SET, NUMBER_SET, BYTEBUFFER_SET, ATTRIBUTE_MAP, ATTRIBUTE_LIST, BOOL, NULLVALUE
        switch (obj)
        {
        case Aws::DynamoDB::Model::ValueType::STRING:
            return val.GetS();
            break;
        case Aws::DynamoDB::Model::ValueType::NUMBER:
            return val.GetN();
            break;
        case Aws::DynamoDB::Model::ValueType::BOOL:
            return val.GetB() == 1 ? "TRUE" : "FALSE";
            break;
        case Aws::DynamoDB::Model::ValueType::NULLVALUE:
            return "";
            break;
        default:
            return "error";
        }
    }

    std::map<std::string, std::string>* dynamodb_base::GetItems(const std::string tableName,
        const std::string partitionKey,
        const std::string partitionValue) {
        Aws::DynamoDB::Model::GetItemRequest request;

        //base map
        std::map<std::string, std::string>* map_base = new std::map<std::string, std::string>;

        // Set up the request.
        request.SetTableName(tableName);
        request.AddKey(partitionKey,
            Aws::DynamoDB::Model::AttributeValue().SetN(partitionValue));

        // Retrieve the item's fields and values.
        const Aws::DynamoDB::Model::GetItemOutcome& outcome = DynamoDB_clientRef->GetItem(request);
        if (outcome.IsSuccess()) {

            // Reference the retrieved fields/values.
            const Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>& item = outcome.GetResult().GetItem();
            if (!item.empty()) {
                map_base->insert(std::make_pair("ExecCode", "1"));
                // Output each retrieved field and its value.
                for (const auto& i : item)
                {
                    map_base->insert(std::make_pair(i.first, AWSDynamoDbResultParser(i.second.GetType(), i.second)));
                }
                map_base->insert(std::make_pair("ErrorMessage", ""));
            }
            else {
                map_base->insert(std::make_pair("ExecCode", "-1"));
                map_base->insert(std::make_pair("ErrorMessage", "User Not found , try contacting your CC"));
                return map_base;
            }
        }
        else {
            map_base->insert(std::make_pair("ExecCode", "0"));
            map_base->insert(std::make_pair("ErrorMessage", "Connectivity error , please try again later"));
            return map_base;
        }

        return map_base;
    }


    std::vector<std::string> dynamodb_base::ListTables()
    {

        std::vector<std::string> result;

        Aws::DynamoDB::Model::ListTablesRequest listTablesRequest;

        listTablesRequest.SetLimit(50);

        do {
            const Aws::DynamoDB::Model::ListTablesOutcome& outcome = DynamoDB_clientRef->ListTables(listTablesRequest);

            if (!outcome.IsSuccess())
            {
                return {outcome.GetError().GetMessage()};
;           }

            for (const auto& tableName : outcome.GetResult().GetTableNames())
                result.push_back(tableName);

            listTablesRequest.SetExclusiveStartTableName(outcome.GetResult().GetLastEvaluatedTableName());

        } while (!listTablesRequest.GetExclusiveStartTableName().empty());

        return result;
    }
}