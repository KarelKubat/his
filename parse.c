#include "his.h"

/* https://en.wikipedia.org/wiki/PJW_hash_function */
static int hashpjw(char const *s) {
  unsigned long h = 0, high;
  while (*s) {
    h = (h << 4) + *s++;
    if ( (high = h & 0xf0000000) )
      h ^= high >> 24;
    h &= ~high;
  }
  return (int)h;
}

static void check_ac(int ac, int required) {
  if (ac < required)
    error("there are not enough parameters, (at least) %d required", required);
}

static void parse_format_1(int ac, char **av, CmdToAdd *cmd) {
  int year, month, day, hour, min, sec, offset = 0;
  struct tm tm_stamp;
  int cmd_index, i;
  char ch1, ch2;
  char *stamp1, *stamp2;
  int off_hrs, off_min;

  /* AV[0]: bash history number, ignored */

  /* AV[1]: Grab the YYYY-MM-DD, expected leader [ */
  check_ac(ac, 2);
  if (sscanf(av[1], "%c%4d-%2d-%2d", &ch1, &year, &month, &day) < 3 &&
      ch1 == '[')
    error("expected '[YYYY-MM-DD' format, got %s instead", av[1]);
  msg("YYYY-MM-DD: %4.4d-%2.2d-%2.2d", year, month, day);

  /* AV[2] (maybe [3] too): Grab the HH:MM:SS or HH:MM:SS +/-OFFSET] */
  cmd_index = 3;  /* Assume no offset */
  check_ac(ac, 3);
  if (sscanf(av[2], "%2d:%2d:%2d%c", &hour, &min, &sec, &ch1) >= 3 &&
      ch1 == ']') {
    msg("HH:MM:SS: %2.2d:%2.2d:%2.2d, no offset", hour, min, sec);
  } else {
    if (sscanf(av[2], "%2d:%2d:%2d", &hour, &min, &sec) < 3)
      error("expected HH:MM:DD or HH:MM:DD], got %s", av[2]);
    msg("HH:MM:SS: %2.2d:%2.2d:%2.2d", hour, min, sec);
    check_ac(ac, 4);
    
    /* Expect the offset now as +HHMM or -HHMM. */
    if (sscanf(av[3], "%c%d%c", &ch1, &offset, &ch2) >= 3 &&
        ch1 == '+' && ch2 == ']') {
      msg("offset: +%d", offset);
      cmd_index = 4;
      /* Offset +0200 means we are 2 hours AHEAD of GMC. Invert to negative
         so it will be subtracted below. */
      offset = -1 * offset;
    } else if (sscanf(av[3], "%c%d%c", &ch1, &offset, &ch2) >= 3
               && ch1 == '-' && ch2 == ']') {
      msg("offset: -%d", offset);
      cmd_index = 4;
    } else
      error("expected +OFFSET] or -OFFSET], got %s", av[3]);
  }

  /* Build up the timestamp so far and apply the offset */
  tm_stamp.tm_sec  = sec;
  tm_stamp.tm_min  = min;
  tm_stamp.tm_hour = hour;
  tm_stamp.tm_mday = day;
  tm_stamp.tm_mon  = month - 1;
  tm_stamp.tm_year = year - 1900;
  cmd->timestamp = gm_mktime(&tm_stamp);
  stamp1 = gm_timestamp(cmd->timestamp);
  /* Offset +0230 means 2.5 hours, not 2.3 hours. */
  off_hrs = offset / 100;
  off_min = offset % 100;
  cmd->timestamp += off_hrs * 3600 + off_min * 60;
  stamp2 = gm_timestamp(cmd->timestamp);

  msg("parsed timestamp: %s, with offset calculation: %s", stamp1, stamp2);
  free(stamp1);
  free(stamp2);

  /* AV[cmd_index] and further is the cmd to add */
  check_ac(ac, cmd_index + 1);
  cmd->ac = ac - cmd_index;
  cmd->av = (char **)xmalloc(cmd->ac * sizeof(char*));
  for (i = cmd_index; i < ac; i++)
    cmd->av[i - cmd_index] = av[i];
}

static void parse_format_2(int ac, char **av, CmdToAdd *cmd) {
  check_ac(ac, 2);

  cmd->timestamp = str2timestamp(av[0]);
  cmd->av = av + 1;
  cmd->ac = ac - 1;
}

static void parse_format_3(int ac, char **av, CmdToAdd *cmd) {
  check_ac(ac, 2);

  if (sscanf(av[0], "%ld", &(cmd->timestamp)) < 0)
    error("UTC timestamp (number) expected, got %s", av[0]);
  cmd->av = av + 1;
  cmd->ac = ac - 1;
}

void parse(int ac, char **av, CmdToAdd *cmd) {
  char *full, *stamp;

  full = full_command(ac, av);
  msg("parsing for addition: %s", full);
  free(full);

  switch (format) {
    case 1:
      parse_format_1(ac, av, cmd);
      break;
    case 2:
      parse_format_2(ac, av, cmd);
      break;
    case 3:
      parse_format_3(ac, av, cmd);
      break;
    default:
      error("internal error in parse, --format %d not yet implemented", format);
      break;
  }

  full = full_command(cmd->ac, cmd->av);
  stamp = gm_timestamp(cmd->timestamp);
  cmd->hash = hashpjw(full);
  msg("cmd parsed: timestamp=%s, cmd=[%s], hash=%d", stamp, full, cmd->hash);
  free(full);
  free(stamp);
}
