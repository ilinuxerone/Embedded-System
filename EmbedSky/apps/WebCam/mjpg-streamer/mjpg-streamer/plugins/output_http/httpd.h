/*******************************************************************************
#                                                                              #
#      MJPG-streamer allows to stream JPG frames from an input-plugin          #
#      to several output plugins                                               #
#                                                                              #
#      Copyright (C) 2007 Tom Stöveken                                         #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; version 2 of the License.                      #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/

#define IO_BUFFER 256
#define BUFFER_SIZE 1024

/* the boundary is used for the M-JPEG stream, it separates the multipart stream of pictures */
#define BOUNDARY "boundarydonotcross"

/*
 * this defines the buffer size for a JPG-frame
 * selecting to large values will allocate much wasted RAM for each buffer
 * selecting to small values will lead to crashes due to to small buffers
 */
#define MAX_FRAME_SIZE (256*1024)
#define TEN_K (10*1024)

/*
 * Standard header to be send along with other header information like mimetype.
 *
 * The parameters should ensure the browser does not cache our answer.
 * A browser should connect for each file and not serve files from his cache.
 * Using cached pictures would lead to showing old/outdated pictures
 * Many browser seem to ignore, or at least not always obey those headers
 * since i observed caching of files from time to time.
 */
#define STD_HEADER "Connection: close\r\n" \
                   "Server: MJPG-Streamer/0.2\r\n" \
                   "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n" \
                   "Pragma: no-cache\r\n" \
                   "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n"

/*
 * Maximum number of server sockets (i.e. protocol families) to listen.
 */
#define MAX_SD_LEN 50

/*
 * Only the following fileypes are supported.
 *
 * Other filetypes are simply ignored!
 * This table is a 1:1 mapping of files extension to a certain mimetype.
 */
static const struct {
  const char *dot_extension;
  const char *mimetype;
} mimetypes[] = {
  { ".html", "text/html" },
  { ".htm",  "text/html" },
  { ".css",  "text/css" },
  { ".js",   "text/javascript" },
  { ".txt",  "text/plain" },
  { ".jpg",  "image/jpeg" },
  { ".jpeg", "image/jpeg" },
  { ".png",  "image/png"},
  { ".gif",  "image/gif" },
  { ".ico",  "image/x-icon" },
  { ".swf",  "application/x-shockwave-flash" },
  { ".cab",  "application/x-shockwave-flash" },
  { ".jar",  "application/java-archive" }
};

/*
 * mapping between command string and command type
 * it is used to find the command for a certain string
 */
static const struct {
  const char *string;
  const in_cmd_type cmd;
} in_cmd_mapping[] = {
  { "reset", IN_CMD_RESET },
  { "reset_pan_tilt", IN_CMD_RESET_PAN_TILT },
  { "pan_set", IN_CMD_PAN_SET },
  { "pan_plus", IN_CMD_PAN_PLUS },
  { "pan_minus", IN_CMD_PAN_MINUS },
  { "tilt_set", IN_CMD_TILT_SET },
  { "tilt_plus", IN_CMD_TILT_PLUS },
  { "tilt_minus", IN_CMD_TILT_MINUS },
  { "saturation_plus", IN_CMD_SATURATION_PLUS },
  { "saturation_minus", IN_CMD_SATURATION_MINUS },
  { "contrast_plus", IN_CMD_CONTRAST_PLUS },
  { "contrast_minus", IN_CMD_CONTRAST_MINUS },
  { "brightness_plus", IN_CMD_BRIGHTNESS_PLUS },
  { "brightness_minus", IN_CMD_BRIGHTNESS_MINUS },
  { "gain_plus", IN_CMD_GAIN_PLUS },
  { "gain_minus", IN_CMD_GAIN_MINUS },
  { "focus_plus", IN_CMD_FOCUS_PLUS },
  { "focus_minus", IN_CMD_FOCUS_MINUS },
  { "focus_set", IN_CMD_FOCUS_SET },
  { "led_on", IN_CMD_LED_ON },
  { "led_off", IN_CMD_LED_OFF },
  { "led_auto", IN_CMD_LED_AUTO },
  { "led_blink", IN_CMD_LED_BLINK },
  { "exposure_manual", IN_CMD_EXPOSURE_MANUAL },
  { "exposure_auto", IN_CMD_EXPOSURE_AUTO },
  { "exposure_shutter_prio", IN_CMD_EXPOSURE_SHUTTER_PRIO },
  { "exposure_aperture_prio", IN_CMD_EXPOSURE_APERTURE_PRIO }
};

/* mapping between command string and command type */
static const struct {
  const char *string;
  const out_cmd_type cmd;
} out_cmd_mapping[] = {
  { "hello_output", OUT_CMD_HELLO },
  { "store", OUT_CMD_STORE }
};

/* mapping between command string and command type */
static const struct {
  const char *string;
  const control_cmd_type cmd;
} control_cmd_mapping[] = {
  { "reconfigure_input", CONTROL_CMD_RECONFIGURE_INPUT }
};

/* the webserver determines between these values for an answer */
typedef enum { A_UNKNOWN, A_SNAPSHOT, A_STREAM, A_COMMAND, A_FILE } answer_t;

/*
 * the client sends information with each request
 * this structure is used to store the important parts
 */
typedef struct {
  answer_t type;
  char *parameter;
  char *client;
  char *credentials;
} request;

/* the iobuffer structure is used to read from the HTTP-client */
typedef struct {
  int level;              /* how full is the buffer */
  char buffer[IO_BUFFER]; /* the data */
} iobuffer;

/* store configuration for each server instance */
typedef struct {
  int port;
  char *credentials;
  char *www_folder;
  char nocommands;
} config;

/* context of each server thread */
typedef struct {
  int sd[MAX_SD_LEN];
  int sd_len;
  int id;
  globals *pglobal;
  pthread_t threadID;

  config conf;
} context;

/*
 * this struct is just defined to allow passing all necessary details to a worker thread
 * "cfd" is for connected/accepted filedescriptor
 */
typedef struct {
  context *pc;
  int fd;
} cfd;

/* prototypes */
void *server_thread(void *arg);
void send_error(int fd, int which, char *message);









