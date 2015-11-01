/*
 Jerry Belich (@j3rrytron) - Prototyping for Play
 http://jerrytron.com/pfp - segment 11 - Making Connected Games
*/

// The voltage we are supplying the motor board.
double batteryVolts = 6.0;
// The voltage the motors are designed to run at.
double motorVolts = 6.0;

PhoBot p = PhoBot(batteryVolts, motorVolts);

// Error Code Return Values
const int8_t kErrorUnknown = -1;
const int8_t kErrorQueueFull = -2;
const int8_t kErrorNoCommand = -3;
const int8_t kErrorBadCommand = -4;
const int8_t kSuccessCmdRun = 1;
const int8_t kSuccessCmdQueued = 2;

// The maximum commands to queue.
const uint8_t kCommandQueueSize = 20;
// Default values.
const String kDefaultSpeed = "25";
const uint8_t kDefaultDuration = 1;

// Is the car currently driving?
bool _driving;
// Are commands currently running?
bool _running;
// Is the command loop paused?
bool _paused;
// The current commands duration.
uint32_t _duration;
// Save duration for paused state.
uint32_t _pausedDur;
// How much time has elapsed during the current command.
elapsedMillis _timeElapsed;
// Command queue.
String _cmdQueue[kCommandQueueSize];
// Current command.
String _command;
// Current index available to save a new command.
uint8_t _saveIndex;
// Current command index.
uint8_t _cmdIndex;
// Commands remaining on the queue.
uint8_t _cmdCount;


void setup() {
    // Setup serial debugging.
    Serial.begin(9600);
    
    // Register a function that can be called externally.
    Particle.function("command", command);
    
    // Initialize values.
    _driving = false;
    _running = false;
    _paused = false;
    _saveIndex = 0;
    _cmdIndex = 0;
    _cmdCount = 0;
    _pausedDur = 0;
}

void loop() {
    // Don't 
    if (_paused) {
        return;
    }
    // If commands are running, and duration has a value (0 = infinite),
    // then check if the duration time has elapsed and the command is done.
    if ((_running && _duration) && _timeElapsed >= _duration) {
        Serial.println("Command complete.");
        // If there are more commands, run the next one.
        if (_cmdCount > 0) {
            Serial.println("Running next command.");
            runNextCmd();
        } else { // Otherwise stop the car, no commands are running.
            Serial.println("Command queue complete.");
            // No more commands to run.
            runCommand("stop-0");
            _running = false;
        }
    }
}

void resetCommands() {
    // Stop the car and reinitialize values.
    runCommand("stop-0");
    _driving = false;
    _running = false;
    _cmdIndex = 0;
    _cmdCount = 0;
    _saveIndex = 0;
}

int8_t command(String aCmd) {
    Serial.println("Cmd: " + aCmd);
    
    // If from twitter, will have the twitter name in front of the body.
    if (aCmd.charAt(0) == '@') {
        // Remove the twitter name, get the text body.
        aCmd = aCmd.substring(aCmd.indexOf(' ') + 1);
    }
    
    // Cleanup any whitespace.
    aCmd.trim();
    
    // Admin command incoming.
    if (aCmd.charAt(0) == '*') {
        return adminCommand(aCmd.substring(1));
    }
    
    // Should command run immediately?
    bool runNow = false;
    if (aCmd.charAt(0) == '!') {
        // Pre-pending a command with ! means replace
        // the current command, and start running.
        runNow = true;
        aCmd = aCmd.substring(1);
        _cmdCount++;
        _cmdQueue[_cmdIndex] = aCmd;
    } else {
        // Check if the queue is already full.
        if (_cmdCount == kCommandQueueSize) {
            return kErrorQueueFull;
        }
        // Put the command in the queue.
        _cmdQueue[_saveIndex] = aCmd;
        _saveIndex++;
        _cmdCount++;
        
        // Circular buffer, so when we reach the end, reset the index to 0.
        if (_saveIndex >= kCommandQueueSize) {
            _saveIndex = 0;
        }
    }

    int8_t status = kSuccessCmdQueued;
    
    // If we aren't running a command, or we got a priority command, run!
    if (!_running || runNow) {
        // Get the return status from running the command.
        status = runNextCmd();
    }
    return status;
}

int8_t runNextCmd() {
    // Error if there are no commands in the queue.
    if (_cmdCount == 0) {
        return kErrorNoCommand;   
    }
    // Retrieve the current command to run.
    _command = _cmdQueue[_cmdIndex];
    // Clear out the command now that we are running it.
    _cmdQueue[_cmdIndex] = "";
    // Move to the next command index.
    _cmdIndex++;
    if (_cmdIndex == kCommandQueueSize) {
        _cmdIndex = 0;
    }
    // One less command on the queue.
    _cmdCount--;
    
    return runCommand(_command);
}

int8_t runCommand(String aCmd) {
    // Direction: forward, back, left, right, stop
    // Duration in Seconds: 0-100, (0 = no timer)
    // Speed: 0-100
    // Format: <direction>-<duration>-<speed>, (stop -or- stop-5 -or- forward-5-100, etc)
    
    // Get the index of the first dash (right after direction string).
    int8_t durIndex = aCmd.indexOf('-');
    String dir = "";
    String speed = kDefaultSpeed;
    uint8_t dur = kDefaultDuration;
    
    // If there is no dash, we only have a single command (stop).
    if (durIndex == -1) {
        dir = aCmd;
    } else {
        dir = aCmd.substring(0, durIndex);
        
        // Move one position past the dash to the next value.
        durIndex++;
        int8_t spdIndex = aCmd.indexOf('-', durIndex);
        // Make sure there's a next value (could have just been a stop).
        if (spdIndex > -1) {
            dur = aCmd.substring(durIndex, spdIndex).toInt();
            speed = aCmd.substring(spdIndex + 1);
        } else {
            dur = aCmd.substring(durIndex).toInt();
        }
    }
    
    // Get the return code.
    int8_t status = setMotorCmd(dir, dur, speed);
    if (status > 0) { // Was a success.
        // We are now running a command, if it wasn't already true.
        _running = true;
    }
    
    return status;
}

int8_t setMotorCmd(String aDir, uint8_t aDur, String aSpeed) {
    Serial.println("Dir: " + aDir);
    Serial.print("Dur: ");
    Serial.println(aDur);
    Serial.println("Speed: " + aSpeed);
    
    uint32_t dur = 0;
    // If we got a duration (seconds), convert it to milliseconds.
    if (aDur) {
        dur = aDur * 1000;
    }
    
    // Take our command and format it for the PhoBot library.
    if (aDir.equalsIgnoreCase("forward")) {
        p.setMotors("M3-F-" + aSpeed);
        p.setMotors("M4-F-" + aSpeed);
        _driving = true;
    } else if (aDir.equalsIgnoreCase("back")) {
        p.setMotors("M3-B-" + aSpeed);
        p.setMotors("M4-B-" + aSpeed);
        _driving = true;
    } else if (aDir.equalsIgnoreCase("left")) {
        p.setMotors("M3-B-" + aSpeed);
        p.setMotors("M4-F-" + aSpeed);
        _driving = true;
    } else if (aDir.equalsIgnoreCase("right")) {
        p.setMotors("M3-F-" + aSpeed);
        p.setMotors("M4-B-" + aSpeed);
        _driving = true;
    } else if (aDir.equalsIgnoreCase("stop")) {
        p.setMotors("M3-S");
        p.setMotors("M4-S");
        _driving = false;
    } else {
        return kErrorBadCommand;
    }
    
    _duration = dur;
    _timeElapsed = 0;
    
    return kSuccessCmdRun;
}

int8_t adminCommand(String aCmd) {
    if (aCmd.charAt(0) == 'x') {
        // Clear command queue.
        resetCommands();
    } else if (aCmd.charAt(0) == 'n') {
        // Next driving command.
        runNextCmd();
    } else if (aCmd.charAt(0) == 'p') {
        if (_paused) {
            runCommand(_command);
            _duration = _pausedDur;
        } else {
            _pausedDur = _duration - _timeElapsed;
            runCommand("stop-0");
        }
        _paused = !_paused;
    }
    return 1;
}