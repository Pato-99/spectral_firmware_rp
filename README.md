
# Serial API

| Command       | Description        |
| ------------- | ------------------ |
| "M <id> ON\n" | power up the motor |
| "M <id> OFF\n" | power off the motor |
| "M <id> HOM\n" | perform homing sequence |
| "M <id> ABS <steps>\n" | move motor to absolute position in <steps> |
| "M <id> REL <steps>\n" | move motor by relative <steps> from current position |
| "M <id> SPD <speed>\n" | set maximum speed in steps per second |
| "M <id> ACC <acceleration>\n" | set acceleration in steps per second^2 |
| "M <id> SG <sgthresh>\n" | set SG THRESHOLD value of TMC2209 driver (used for stallguard) |
| "S ON\n" | power on the servo |
| "S OFF\n" | power off the servo |
| "S IN\n | turn the servo from the optical axis (hide the neon) |
| "S OUT\n" | turn the servo to the optical axis (show the neon) |
| "R ON\n" | turn on the relay |
| "R OFF\n" | turn off the raley |

