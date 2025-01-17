#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include "click.hpp"

/*
 * Which button to press.
 */
static int button = 1;

/*
 * Delay between clicks in milliseconds.
 */
static uint64_t delay = 50;

/*
 * The amount of pixels you can move before it performs the on-leave-area
 * action.
 * seraph is pretty cool.
 */
static int area = 0;

/*
 * How often to check for mouse position changes. This is a costly operation
 * and in many cases is not important to be checked as often as clicking is.
 * It's in milliseconds.
 */
static uint64_t position_check_delay = 500;

/*
 * This is a program that lets the user choose a position on the screen where
 * mouse clicks will be generated quickly in succession until the mouse is
 * moved away.
 */
int main(int argc, char *argv[])
{
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "show help and exit")
		("version,v", "show version and exit")
		("button", boost::program_options::value<std::string>()->default_value("left"), "which button to press")
		("delay", boost::program_options::value<int>()->default_value(delay), "delay between clicks, in milliseconds")
		("on-leave-area", boost::program_options::value<std::string>()->default_value("exit"), "the action to perform on leaving the click area (exit, pause)")
		("area", boost::program_options::value<int>()->default_value(area), "the amount of pixels you can move before it performs the on-leave-area action")
		("position-check-delay", boost::program_options::value<int>()->default_value(position_check_delay), "delay between checking mouse position changes, in milliseconds");

	boost::program_options::variables_map vm;

	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return EXIT_SUCCESS;
	}

	if (vm.count("version")) {
    std::cout << "click4ever v" << "1.3.0" << std::endl;
		return EXIT_SUCCESS;
	}

	if (vm.count("button")) {
		std::string requested = vm.at("button").as<std::string>();

		if (requested == "left") {
			button = 1;
		} else if (requested == "right") {
			button = 3;
		} else if (requested == "middle") {
			button = 2;
		} else {
			std::cerr << "Button '" << requested << "' not recognized." << std::endl;
			return EXIT_FAILURE;
		}
	}

	if (vm.count("delay")) {
		int requested = vm.at("delay").as<int>();

		if (requested < 0) {
			std::cerr << "Delay must be a positive number." << std::endl;
			return EXIT_FAILURE;
		}

		if (requested > (1000 * 60 * 24 * 7)) {
			std::cerr << "The maximum allowed value for delay is 7 days." << std::endl;
			return EXIT_FAILURE;
		}

		delay = requested;
	}

	if (vm.count("position-check-delay")) {
		int requested = vm.at("position-check-delay").as<int>();

		if (requested < 0) {
			std::cerr << "Position check delay must be a positive number." << std::endl;
			return EXIT_FAILURE;
		}

		if (requested > (1000 * 60 * 24 * 7)) {
			std::cerr << "The maximum allowed value for position check delay is 7 days." << std::endl;
			return EXIT_FAILURE;
		}

		position_check_delay = requested;
	}

	if (vm.count("area")) {
		int requested = vm.at("area").as<int>();

		if (requested < 0) {
			std::cerr << "Movement threshold must be a positive number." << std::endl;
			return EXIT_FAILURE;
		}

		if (requested > 32000) {
			std::cerr << "Movement threshold is not allowed to be greater than 32 thousand." << std::endl;
			return EXIT_FAILURE;
		}

		area = requested;
	}

	ClickOnLeaveArea click_on_leave_area;

	if (vm.count("on-leave-area")) {
		std::string requested = vm.at("on-leave-area").as<std::string>();

		boost::algorithm::to_lower(requested);

		if (requested == "exit") {
			click_on_leave_area = ClickOnLeaveArea::Exit;
		} else if (requested == "pause") {
			click_on_leave_area = ClickOnLeaveArea::Pause;

			if (area == 0) {
				std::cerr << "You must specify the size of the area." << std::endl;
				return EXIT_FAILURE;
			}
		} else {
			std::cerr << "Option for on-leave-area not recognized." << std::endl;
			return EXIT_FAILURE;
		}
	}

	click(button, delay, position_check_delay, area, click_on_leave_area);

	return EXIT_SUCCESS;
}
