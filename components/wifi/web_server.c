#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_err.h"
#include "esp_log.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"

#include "cJSON.h"

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN)
#define MAX_FILE_SIZE   (200*1024)
#define MAX_FILE_SIZE_STR "200KB"
#define SCRATCH_BUFSIZE  8192

extern unsigned char wifi_ssid[32];
extern unsigned char wifi_password[64];

struct file_server_data {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
};

static const char *TAG = "file_server";

static esp_err_t send_wifi_handler(httpd_req_t *req)
{
    char buf[128];
    int ret, remaining = req->content_len;
    bzero(buf, sizeof(buf));

    while (remaining > 0) {
        ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
        if (ret <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            httpd_resp_sendstr(req, "Failed to receive data");
            return ESP_FAIL;
        }
        remaining -= ret;
    }

    cJSON *root = cJSON_Parse(buf);
    if (root == NULL) {
        httpd_resp_sendstr(req, "Failed to parse JSON data");
        return ESP_FAIL;
    }

    char *ssid = cJSON_GetObjectItem(root, "wifi_ssid")->valuestring;
    char *passwd = cJSON_GetObjectItem(root, "wifi_passwd")->valuestring;

    memcpy(wifi_ssid,ssid,strlen(ssid));
    memcpy(wifi_password,passwd,strlen(passwd));

    cJSON_Delete(root);

    printf("\r\nwifi_ssid: %s", wifi_ssid);
    printf("\r\nwifi_passwd: %s\r\n", wifi_password);

    httpd_resp_sendstr(req, "WiFi configuration successful");
    return ESP_OK;
}


static esp_err_t root_get_handler(httpd_req_t *req)
{
    extern const unsigned char upload_script_start[] asm("_binary_upload_script_html_start");
    extern const unsigned char upload_script_end[] asm("_binary_upload_script_html_end");
    const size_t upload_script_size = (upload_script_end - upload_script_start);
    httpd_resp_send(req, (const char *)upload_script_start, upload_script_size);
    return ESP_OK;
}

esp_err_t start_web_server()
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "Starting HTTP Server");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start file server!");
        return ESP_FAIL;
    }

    httpd_uri_t wifi_data = {
        .uri       = "/wifi_data",
        .method    = HTTP_POST,
        .handler   = send_wifi_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &wifi_data);

    httpd_uri_t root = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = root_get_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &root);

    return ESP_OK;
}
