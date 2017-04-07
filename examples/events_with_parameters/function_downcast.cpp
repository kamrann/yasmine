//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
// This file is part of the Seadex yasmine ecosystem (http://yasmine.seadex.de).                    //
// Copyright (C) 2016-2017 Seadex GmbH                                                              //
//                                                                                                  //
// Licensing information is available in the folder "license" which is part of this distribution.   //
// The same information is available on the www @ http://yasmine.seadex.de/Licenses.html.           //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


#include "function_downcast.hpp"

#include <iostream>

#include "events.hpp"


namespace
{


typedef sxe::SX_UNIQUE_PTR< sxy::sync_state_machine > state_machine_uptr;


#ifndef SX_CPP03_BOOST
void do_something_event_0_parameters( const sxy::completion_event& _event, sxy::event_collector& _event_collector )
{		
	SX_UNUSED_PARAMETER( _event );
	SX_UNUSED_PARAMETER( _event_collector );
	std::cout << "Completion event has no parameters." << std::endl;
}
#else
void do_something_event_0_parameters( const sxy::event& _event )
{
	const sxy::completion_event* specialized_event = dynamic_cast< const sxy::completion_event* >( &_event );
	if( specialized_event )
	{
		std::cout << "Completion event has no parameters." << std::endl;
	}
	else
	{
		throw sxe::exception( "Invalid event type!" );
	}
}
#endif


#ifndef SX_CPP03_BOOST
void do_something_event_1_parameter( const examples::event_1& _event )
{		
	std::cout << "Parameter of event " << _event.get_name() << " is: " << _event.get_param() << std::endl;
}
#else
void do_something_event_1_parameter( const sxy::event& _event )
{
	const examples::event_1* specialized_event = dynamic_cast< const examples::event_1* >( &_event );
	if( specialized_event )
	{
		std::cout << "Parameter of event " << specialized_event->get_name() << " is: " << specialized_event->get_param() << std::endl;
	}
	else
	{
		throw sxe::exception( "Invalid event type!" );
	}
}
#endif


#ifndef SX_CPP03_BOOST
void do_something_event_2_parameters( const examples::event_2& _event, sxy::event_collector& _event_collector )
{		
	SX_UNUSED_PARAMETER( _event_collector );
	std::cout << "Parameters of event " << _event.get_name() << " are:\n" << _event.get_param_1() << "\n" <<
		_event.get_param_2() << std::endl;
}
#else
void do_something_event_2_parameters( const sxy::event& _event )
{	
	const examples::event_2* specialized_event = dynamic_cast< const examples::event_2* >( &_event );
	if( specialized_event )
	{
		std::cout << "Parameters of event " << specialized_event->get_name() << " are:\n" 
			<< specialized_event->get_param_1() << "\n" << specialized_event->get_param_2() << std::endl;
	}
	else
	{
		const sxy::completion_event* specialized_event2 = dynamic_cast< const sxy::completion_event* >( &_event );
		if( specialized_event2 )
		{
			do_something_event_0_parameters( _event );
		}
		else
		{
			throw sxe::exception( "Invalid event type!" );
		}
	}
}
#endif


state_machine_uptr setup_state_machine2( const std::string& _name )
{
	state_machine_uptr state_machine = SX_MAKE_UNIQUE< sxy::sync_state_machine >( _name );
	sxy::composite_state& root_state = state_machine->get_root_state();
	sxy::region& main_region = root_state.add_region( "main region" );
	sxy::initial_pseudostate& initial_pseudostate = main_region.add_initial_pseudostate( "initial" );
#ifndef SX_CPP03_BOOST
	sxy::simple_state& simple_state_1 = main_region.add_simple_state( "1",
		Y_BEHAVIOR_FUNCTION2( &do_something_event_0_parameters, &do_something_event_2_parameters ),
		Y_BEHAVIOR_FUNCTION2( &do_something_event_0_parameters, &do_something_event_2_parameters ),
		Y_BEHAVIOR_FUNCTION2( &do_something_event_1_parameter ) );
	sxy::simple_state& simple_state_2 = main_region.add_simple_state( "2", 
		Y_BEHAVIOR_FUNCTION2( &do_something_event_1_parameter ),
		Y_BEHAVIOR_FUNCTION2( &do_something_event_1_parameter ),
		Y_BEHAVIOR_FUNCTION2( &do_something_event_2_parameters ) );

	state_machine->add_transition( sxy::Y_COMPLETION_EVENT_ID, initial_pseudostate, simple_state_1,
		Y_BEHAVIOR_FUNCTION2( &do_something_event_0_parameters ) );
	state_machine->add_transition( EVENT_1, simple_state_1, simple_state_2,
		Y_BEHAVIOR_FUNCTION2( &do_something_event_1_parameter ) );
	state_machine->add_transition( EVENT_2, simple_state_2, simple_state_1,
		Y_BEHAVIOR_FUNCTION2( &do_something_event_2_parameters ) );
#else
	sxy::simple_state& simple_state_1 = main_region.add_simple_state( "1",
		sxy::behavior_function( sxe::bind( &do_something_event_2_parameters, sxe::_1 ) ),
		sxy::behavior_function( sxe::bind( &do_something_event_2_parameters, sxe::_1 ) ),
		sxy::behavior_function( sxe::bind( &do_something_event_1_parameter, sxe::_1 ) ) );
	sxy::simple_state& simple_state_2 = main_region.add_simple_state( "2",
		sxy::behavior_function( sxe::bind( &do_something_event_1_parameter, sxe::_1 ) ),
		sxy::behavior_function( sxe::bind( &do_something_event_1_parameter, sxe::_1 ) ),
		sxy::behavior_function( sxe::bind( &do_something_event_2_parameters, sxe::_1 ) ) );

	state_machine->add_transition( sxy::Y_COMPLETION_EVENT_ID, initial_pseudostate, simple_state_1,
		sxy::behavior_function( sxe::bind( &do_something_event_0_parameters, sxe::_1 ) ) );
	state_machine->add_transition( EVENT_1, simple_state_1, simple_state_2,
		sxy::behavior_function( sxe::bind( &do_something_event_1_parameter, sxe::_1 ) ) );
	state_machine->add_transition( EVENT_2, simple_state_2, simple_state_1,
		sxy::behavior_function( sxe::bind( &do_something_event_2_parameters, sxe::_1 ) ) );
#endif

	
	return( sxe::move( state_machine ) );
}


}


namespace examples
{


void use_function_downcast()
{
	state_machine_uptr state_machine = setup_state_machine2( "downcast_event_state_machine" );
	state_machine->run();
	state_machine->fire_event( event_1::create( "parameter 1 test" ) );
	state_machine->fire_event( event_2::create( 2, 3.4 ) );
	state_machine->halt();
}


}
