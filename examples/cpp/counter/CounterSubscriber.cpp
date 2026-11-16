// Receives the uint64 counter published on topic "counter_topic".

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicData.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicDataFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicPubSubType.hpp>

#include "CounterType.hpp"

using namespace eprosima::fastdds::dds;

int main()
{
    DomainParticipant* participant =
            DomainParticipantFactory::get_instance()->create_participant_with_default_profile(nullptr,
                    StatusMask::none());
    if (participant == nullptr)
    {
        std::cerr << "Participant creation failed" << std::endl;
        return 1;
    }

    DynamicType::_ref_type type = create_counter_type();
    if (!type)
    {
        std::cerr << "Type creation failed" << std::endl;
        return 1;
    }

    TypeSupport type_support(new DynamicPubSubType(type));
    if (RETCODE_OK != type_support.register_type(participant))
    {
        std::cerr << "Type registration failed" << std::endl;
        return 1;
    }

    Subscriber* subscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
    Topic* topic = participant->create_topic("counter_topic", type_support.get_type_name(), TOPIC_QOS_DEFAULT);
    DataReader* reader = subscriber->create_datareader(topic, DATAREADER_QOS_DEFAULT, nullptr);
    if (reader == nullptr)
    {
        std::cerr << "DataReader creation failed" << std::endl;
        return 1;
    }

    DynamicData::_ref_type sample = DynamicDataFactory::get_instance()->create_data(type);
    MemberId counter_id = sample->get_member_id_by_name("counter");

    std::cout << "Waiting for counter samples (Ctrl+C to stop)..." << std::endl;
    while (true)
    {
        SampleInfo info;
        if (RETCODE_OK == reader->take_next_sample(&sample, &info) && info.valid_data)
        {
            uint64_t value = 0;
            if (RETCODE_OK == sample->get_uint64_value(value, counter_id))
            {
                std::cout << "Received counter: " << value << std::endl;
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}
