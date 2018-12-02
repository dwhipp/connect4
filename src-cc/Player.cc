#include "Player.h"

#include <iostream>

std::unique_ptr<Player> Player::New(std::string_view name_spec) {
  if (name_spec.empty()) {
    std::cerr << "Bad player spec <empty>\n";
    return {};
  }

  std::string_view name =
      name_spec.size() == 1
        ? ""
        : name_spec[1] == ':' ? name_spec.substr(2) : name_spec;

  switch (name_spec.front()) {
    case 'h':
      return Player::NewHuman(name.empty() ? "Human" : name);

    case 'b':
      return Player::NewBruteForce(
        (name.empty() ? "Brute Force" : name),
        /*depth=*/ 5,
        /*sharpness=*/ .9999,
        /*discount=*/ .999,
        std::make_unique<std::random_device>());

    case 'm':
      return Player::NewMonteCarlo(
          (name.empty() ? "Monte Carlo" : name),
          /*num_rollouts=*/10000,
          /*exploration=*/std::sqrt(2),
          std::make_unique<std::random_device>());

    default:
      std::cerr << "Bad player spec: " << name_spec << "\n";
      return {};
  }
}
