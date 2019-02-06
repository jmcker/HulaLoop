#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "CLICommon.h"
#include "CLICommands.h"
#include "InteractiveCLI.h"

using namespace hula;
using namespace std;

/**
 * Constuct a new instance of InteractiveCLI.
 *
 * This will not start the command loop.
 */
InteractiveCLI::InteractiveCLI(QCoreApplication *app)
{
    this->app = app;
    this->t = new Transport();
    this->settings = HulaSettings::getInstance();
}

/**
 * Print a warning to the user detailing extra arguments
 * that they provided.
 *
 * @param args Vector of all arguments provided to the command
 * @param numUsed Number of arguments actually used by the command
 */
void InteractiveCLI::unusedArgs(const std::vector<std::string> &args, int numUsed) const
{
    for (size_t i = numUsed - 1; i < args.size(); i++)
    {
        fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, qPrintable(tr("Warning: Unused argument '%1'.").arg(args[i].c_str())));
    }
}

/**
 * Print a warning about missing args to the specified command
 *
 * @param argName Name of the missing argument
 */
void InteractiveCLI::missingArg(const std::string &argName) const
{
    fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, qPrintable(tr("Missing argument '%1'").arg(argName.c_str())));
}

/**
 * Print a warning about a malformed argument.
 *
 * @param argName Name of the malformed argument
 * @param val Value given by user
 * @param type Expected type of value
 */
void InteractiveCLI::malformedArg(const std::string &argName, const std::string &val, const std::string &type) const
{
    fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, qPrintable(tr("Malformed argument '%1'").arg(argName.c_str())));
    fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, qPrintable(tr("'%1' is not a valid %2.").arg(val.c_str(), type.c_str())));
}

/**
 * Start taking in commands.
 *
 * This is an infinite loop.
 */
void InteractiveCLI::start()
{
    std::string line;
    std::string command;
    std::string arg;
    std::vector<std::string> args;

    // Command loop
    while (1)
    {
        command = "";
        arg = "";
        args.clear();

        printf(HL_PROMPT);

        // Get the whole line
        if (std::getline(std::cin, line))
        {
            // Parse the command and args out of the line
            std::istringstream iss(line);
            iss >> command;
            while (iss >> arg)
            {
                args.push_back(arg);
            }
        }
        else
        {
            break;
        }

        HulaCliStatus stat = processCommand(command, args);

        if (stat == HulaCliStatus::HULA_CLI_EXIT)
        {
            break;
        }
    }
}

/**
 * Proccess the command entered by the user.
 *
 * @param command Name of command in short or long form
 * @param args Vector of arguments that should be used with the command
 * @return HulaCliStatus Outcome of the command
 */
HulaCliStatus InteractiveCLI::processCommand(const std::string &command, const std::vector<std::string> &args)
{
    bool success = true;
    HulaCliStatus stat = HulaCliStatus::HULA_CLI_SUCCESS;

    // Take action based on the command
    if (command.size() == 0)
    {
        return HulaCliStatus::HULA_CLI_SUCCESS;
    }
    else if (command == HL_DELAY_TIMER_SHORT || command == HL_DELAY_TIMER_LONG)
    {
        // Make sure the arg exists
        if (args.size() != 0)
        {
            double delay;
            try
            {
                delay = std::stod(args[0], nullptr);
                this->delay = delay;
            }
            catch (std::invalid_argument &e)
            {
                (void)e;

                malformedArg(HL_DELAY_TIMER_ARG1, args[0], "double");
                return HulaCliStatus::HULA_CLI_FAILURE;
            }
        }
        else
        {
            missingArg(HL_DELAY_TIMER_ARG1);
            return HulaCliStatus::HULA_CLI_FAILURE;
        }
    }
    else if (command == HL_RECORD_TIMER_SHORT || command == HL_RECORD_TIMER_LONG)
    {
        // Make sure the arg exists
        if (args.size() != 0)
        {
            double duration;
            try
            {
                duration = std::stod(args[0], nullptr);
                this->duration = duration;
            }
            catch (std::invalid_argument &e)
            {
                (void)e;

                malformedArg(HL_RECORD_TIMER_ARG1, args[0], "double");
                return HulaCliStatus::HULA_CLI_FAILURE;
            }
        }
        else
        {
            missingArg(HL_RECORD_TIMER_ARG1);
            return HulaCliStatus::HULA_CLI_FAILURE;
        }
    }
    else if (command == HL_RECORD_SHORT || command == HL_RECORD_LONG)
    {
        double delay = 0;
        double duration = HL_INFINITE_RECORD;

        // Try to use the args provided to the record command
        // If the args are not provided, this will default to
        // the delay and duration set by the 'delay' and 'duration'
        // commands
        if (args.size() == 2)
        {
            try
            {
                delay = std::stod(args[0], nullptr);
            }
            catch (std::invalid_argument &e)
            {
                (void)e;

                malformedArg(HL_RECORD_ARG1, args[0], "double");
                return HulaCliStatus::HULA_CLI_FAILURE;
            }

            try
            {
                duration = std::stod(args[1], nullptr);
            }
            catch (std::invalid_argument &e)
            {
                (void)e;

                malformedArg(HL_RECORD_ARG2, args[1], "double");
                return HulaCliStatus::HULA_CLI_FAILURE;
            }

            try
            {
                success = t->record(delay, duration);
            }
            catch(const ControlException &ce)
            {
                fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, ce.getErrorMessage().c_str());
                return HulaCliStatus::HULA_CLI_FAILURE;
            }
        }
        else if (args.size() == 1)
        {
            try
            {
                delay = std::stod(args[0], nullptr);
            }
            catch (std::invalid_argument &e)
            {
                (void)e;

                malformedArg(HL_RECORD_ARG1, args[0], "double");
                return HulaCliStatus::HULA_CLI_FAILURE;
            }

            try
            {
                success = t->record(delay, HL_INFINITE_RECORD);
            }
            catch(const ControlException &ce)
            {
                fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, ce.getErrorMessage().c_str());
                return HulaCliStatus::HULA_CLI_FAILURE;
            }
        }
        else
        {
            printf("Using stored delay and duration.\n");
            printf("Delay: %.2f\n", this->delay);
            printf("Duration: %.2f\n", this->duration);

            try
            {
                success = t->record(this->delay, this->duration);
            }
            catch(const ControlException &ce)
            {
                fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, ce.getErrorMessage().c_str());
                return HulaCliStatus::HULA_CLI_FAILURE;
            }

        }
    }
    else if (command == HL_STOP_SHORT || command == HL_STOP_LONG)
    {
        try
        {
            success = t->stop();
        }
        catch(const ControlException &ce)
        {
            fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, ce.getErrorMessage().c_str());
            return HulaCliStatus::HULA_CLI_FAILURE;
        }
    }
    else if (command == HL_PLAY_SHORT || command == HL_PLAY_LONG)
    {
        try
        {
            success = t->play();
        }
        catch(const ControlException &ce)
        {
            fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, ce.getErrorMessage().c_str());
            return HulaCliStatus::HULA_CLI_FAILURE;
        }
    }
    else if (command == HL_PAUSE_SHORT || command == HL_PAUSE_LONG)
    {
        try
        {
            success = t->pause();
        }
        catch(const ControlException &ce)
        {
            fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, ce.getErrorMessage().c_str());
            return HulaCliStatus::HULA_CLI_FAILURE;
        }
    }
    else if (command == HL_EXPORT_SHORT || command == HL_EXPORT_LONG)
    {
        // Make sure the arg exists
        if (args.size() > 0)
        {
            t->exportFile(args[0]);
        }
        else if (this->outputFilePath.size() != 0)
        {
            t->exportFile(this->outputFilePath);
        }
        else
        {
            missingArg(HL_EXPORT_ARG1);
            return HulaCliStatus::HULA_CLI_FAILURE;
        }
    }
    else if (command == HL_DISCARD_SHORT || command == HL_DISCARD_LONG)
    {
        std::string resp = "N";

        // Handle force option
        if (args.size() >= 1 && args[0] == HL_DISCARD_ARG1)
        {
            resp = "Y";
        }
        else
        {
            printf("%s", qPrintable(CLI::tr("Are you sure you want to discard? (y/N): ")));
            std::getline(std::cin, resp);
        }

        if (resp.size() > 0 && (resp[0] == 'Y' || resp[0] == 'y'))
        {
            t->discard();
        }
        else
        {
            printf("%s\n", qPrintable(CLI::tr("Discard cancelled.")));
        }
    }
    else if (command == HL_LIST_SHORT || command == HL_LIST_LONG)
    {
        printDeviceList(t);
    }
    else if (command == HL_INPUT_SHORT || command == HL_INPUT_LONG)
    {
        Device *device = nullptr;
        // Make sure the arg exists
        if (args.size() != 0)
        {
            device = findDevice(t, args[0], (DeviceType)(DeviceType::RECORD | DeviceType::LOOPBACK));
        }
        else
        {
            missingArg(HL_INPUT_ARG1);
            return HulaCliStatus::HULA_CLI_FAILURE;
        }

        // Find device will already have printed a not-found error
        if (device != nullptr)
        {
            bool ret = false;

            try
            {
                ret = t->getController()->setActiveInputDevice(device);
            }
            catch(const AudioException &ae)
            {
                ControlException ce(ae.getErrorCode());

                fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, ce.getErrorMessage().c_str());
                return HulaCliStatus::HULA_CLI_FAILURE;
            }

            if (ret)
            {
                printf("\n%s\n", qPrintable(tr("Input device set to: %1").arg(device->getName().c_str())));
            }
            else
            {
                fprintf(stderr, "\n%s\n", qPrintable(tr("Failed to set input device.")));
            }

            delete device;

            if (!ret)
            {
                return HulaCliStatus::HULA_CLI_FAILURE;
            }
        }
        else
        {
            return HulaCliStatus::HULA_CLI_FAILURE;
        }
    }
    else if (command == HL_OUTPUT_SHORT || command == HL_OUTPUT_LONG)
    {
        Device *device = nullptr;
        // Make sure the arg exists
        if (args.size() != 0)
        {
            device = findDevice(t, args[0], DeviceType::PLAYBACK);
        }
        else
        {
            missingArg(HL_OUTPUT_ARG1);
            return HulaCliStatus::HULA_CLI_FAILURE;
        }

        // Find device will already have printed a not-found error
        if (device != nullptr)
        {
            bool ret = false;
            try
            {
                ret = t->getController()->setActiveOutputDevice(device);
            }
            catch(const AudioException &ae)
            {
                ControlException ce(ae.getErrorCode());

                fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, ce.getErrorMessage().c_str());
                return HulaCliStatus::HULA_CLI_FAILURE;
            }

            if (ret)
            {
                printf("\n%s\n", qPrintable(tr("Output device set to: %1").arg(device->getName().c_str())));
            }
            else
            {
                fprintf(stderr, "\n%s\n", qPrintable(tr("Failed to set output device.")));
            }

            delete device;

            if (!ret)
            {
                return HulaCliStatus::HULA_CLI_FAILURE;
            }
        }
        else
        {
            return HulaCliStatus::HULA_CLI_FAILURE;
        }
    }
    else if (command == HL_PRINT_SHORT || command == HL_PRINT_LONG)
    {
        // Copy the settings stored in InteractiveCLI
        // This is a hack for now
        HulaImmediateArgs localSettings;

        localSettings.delay = std::to_string(this->delay);
        localSettings.duration = std::to_string(this->duration);
        localSettings.outputFilePath = this->outputFilePath;

        printSettings(localSettings);
    }
    else if (command == HL_VERSION_SHORT || command == HL_VERSION_LONG)
    {
        printf("%s v%s\n", HL_CLI_NAME, HL_VERSION_STR);
    }
    else if (command == HL_HELP_SHORT || command == HL_HELP_LONG)
    {
        printInteractiveHelp();
    }
    else if (command == HL_LANG_SHORT || command == HL_LANG_LONG)
    {
        if (args.size() < 1)
        {
            missingArg(HL_LANG_ARG1);
            return HulaCliStatus::HULA_CLI_FAILURE;
        }

        success = settings->loadLanguage(this->app, args[0]);

        if (success)
        {
            printf("%s\n", qPrintable(tr("Translation file successfully loaded.")));
        }
        else
        {
            fprintf(stderr, "%s\n", qPrintable(tr("Could not find translation file for %1.").arg(args[0].c_str())));
            return HulaCliStatus::HULA_CLI_FAILURE;
        }
    }
    else if (command == HL_SYSTEM_SHORT || command == HL_SYSTEM_LONG)
    {
        // Make sure there is a command processor available
        if (system(nullptr))
        {
            // Construct the sys command from args
            std::string sysCommand = "";
            for (int i = 0; i < args.size(); i++)
            {
                sysCommand += args[i] + " ";
            }

            // Run the command
            if (sysCommand.size() > 0)
            {
                int ret = system(sysCommand.c_str());
                printf("\n%s%s", HL_PRINT_PREFIX, qPrintable(tr("System command returned: %1").arg(ret)));
            }
        }
        else
        {
            fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, qPrintable(tr("No system command processor is available.")));
            return HulaCliStatus::HULA_CLI_FAILURE;
        }
    }
    else if (command == HL_EXIT_LONG)
    {
        return HulaCliStatus::HULA_CLI_EXIT;
    }
    else
    {
        fprintf(stderr, "%s%s\n", HL_ERROR_PREFIX, qPrintable(tr("Unrecognized command '%1'").arg(command.c_str())));
        return HulaCliStatus::HULA_CLI_FAILURE;
    }

    // Make sure transport commands succeeded
    if (!success)
    {
        fprintf(stderr, "%s\n", qPrintable(tr("Command failed with value of 'false'.")));
        return HulaCliStatus::HULA_CLI_FAILURE;
    }

    return HulaCliStatus::HULA_CLI_SUCCESS;
}

/**
 * Fetch the state of the internal Transport.
 *
 * @return State of the transport
 */
TransportState InteractiveCLI::getState()
{
    return this->t->getState();
}

/**
 * Set the default output file path used by the CLI.
 * This is used primarily so that the CLI --ouput-file
 * flag can affect the export path.
 */
void InteractiveCLI::setOutputFilePath(const std::string &path)
{
    this->outputFilePath = path;
}

/**
 * Destructor for InteractiveCLI.
 */
InteractiveCLI::~InteractiveCLI()
{
    hlDebugf("InteractiveCLI destructor called\n");

    delete this->t;
}
