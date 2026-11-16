// Shared dynamic type for the counter example: a struct "Counter" with one uint64 member "counter".

#ifndef _FASTDDS_EXAMPLES_COUNTER_COUNTERTYPE_HPP_
#define _FASTDDS_EXAMPLES_COUNTER_COUNTERTYPE_HPP_

#include <fastdds/dds/xtypes/dynamic_types/DynamicType.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilder.hpp>
#include <fastdds/dds/xtypes/dynamic_types/DynamicTypeBuilderFactory.hpp>
#include <fastdds/dds/xtypes/dynamic_types/MemberDescriptor.hpp>

inline eprosima::fastdds::dds::DynamicType::_ref_type create_counter_type()
{
    using namespace eprosima::fastdds::dds;

    TypeDescriptor::_ref_type type_descriptor {traits<TypeDescriptor>::make_shared()};
    type_descriptor->kind(TK_STRUCTURE);
    type_descriptor->name("Counter");

    DynamicTypeBuilder::_ref_type struct_builder =
            DynamicTypeBuilderFactory::get_instance()->create_type(type_descriptor);
    if (!struct_builder)
    {
        return nullptr;
    }

    MemberDescriptor::_ref_type member_descriptor {traits<MemberDescriptor>::make_shared()};
    member_descriptor->name("counter");
    member_descriptor->type(DynamicTypeBuilderFactory::get_instance()->get_primitive_type(TK_UINT64));

    if (RETCODE_OK != struct_builder->add_member(member_descriptor))
    {
        return nullptr;
    }

    return struct_builder->build();
}

#endif // _FASTDDS_EXAMPLES_COUNTER_COUNTERTYPE_HPP_
