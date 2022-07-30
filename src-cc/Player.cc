#include "Player.h"

#include <iostream>
#include <cctype>

std::unique_ptr<Player> Player::New(std::string_view name_spec) {
  if (name_spec.empty()) {
    std::cerr << "Bad player spec <empty>\n";
    return {};
  }

  std::string_view name =
      name_spec.size() == 1
        ? ""
        : name_spec.find(':') == name_spec.npos
          ? name_spec
          : name_spec.substr(name_spec.find(':') + 1);

  std::vector<int> args;
  for (size_t idx = 1;
      idx < name_spec.size() && std::isdigit(name_spec[idx]);
      idx++) {
    args.push_back(std::stoi(std::string{name_spec.substr(idx)}, &idx));
  }
  
  switch (name_spec.front()) {
    case 'h':
      return Player::NewHuman(name.empty() ? "Human" : name);

    case 'b':
      return Player::NewBruteForce(
        (name.empty() ? "Brute Force" : name),
        /*depth=*/ args.empty() ? 5 : args[0],
        /*sharpness=*/ .9999,
        /*discount=*/ .999,
        std::make_unique<std::random_device>());

    case 'm':
      return Player::NewMonteCarlo(
          (name.empty() ? "Monte Carlo" : name),
          /*num_rollouts=*/ args.empty() ? 10000 : args[0],
          /*exploration=*/std::sqrt(2),
          std::make_unique<std::random_device>());

    default:
      std::cerr << "Bad player spec: " << name_spec << "\n";
      return {};
  }
}
