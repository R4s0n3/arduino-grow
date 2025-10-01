#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

class CommandManager {
public:
  static void handle(const char* cmd);
  // Register commands dynamically if needed
};

#endif
