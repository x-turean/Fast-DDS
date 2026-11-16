// Publishes a uint64 counter once per second on topic "counter_topic".

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
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

    Publisher* publisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT);
    Topic* topic = participant->create_topic("counter_topic", type_support.get_type_name(), TOPIC_QOS_DEFAULT);
    DataWriter* writer = publisher->create_datawriter(topic, DATAWRITER_QOS_DEFAULT, nullptr);
    if (writer == nullptr)
    {
        std::cerr << "DataWriter creation failed" << std::endl;
        return 1;
    }

    // Wait until a subscriber is matched so no samples are lost
    std::cout << "Waiting for a subscriber..." << std::endl;
    std::vector<InstanceHandle_t> subscriptions;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        writer->get_matched_subscriptions(subscriptions);
    }
    while (subscriptions.empty());
    std::cout << "Subscriber matched, sending counter" << std::endl;

    DynamicData::_ref_type sample = DynamicDataFactory::get_instance()->create_data(type);
    MemberId counter_id = sample->get_member_id_by_name("counter");

    for (uint64_t counter = 0;; ++counter)
    {
        sample->set_uint64_value(counter_id, counter);
        if (RETCODE_OK != writer->write(&sample))
        {
            std::cerr << "Failed to send counter " << counter << std::endl;
        }
        else
        {
            std::cout << "Sent counter: " << counter << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
