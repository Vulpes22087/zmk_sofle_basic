#include <zephyr/kernel.h>
#include <zmk/display/status_screen.h>
#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/hid.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <lvgl.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_output_status output_status_widget;
static struct zmk_widget_battery_status battery_status_widget;

static bool caps_lock_active = false;
static lv_obj_t *caps_label;

// 更新螢幕上的 CAPS 文字
static void update_caps_status() {
    if (caps_lock_active) {
        lv_label_set_text(caps_label, " [ CAPS ] ");
    } else {
        lv_label_set_text(caps_label, "");
    }
}

// 監聽來自電腦的 HID 指示燈事件（包含 Caps Lock）
static int hid_indicators_listener(const zmk_event_t *eh) {
    caps_lock_active = (zmk_hid_indicators_get_current_profile() & HID_USAGE_LED_CAPS_LOCK) != 0;
    update_caps_status();
    return ZMK_EV_EVENT_BUBBLE;
}
ZMK_LISTENER(hid_indicators, hid_indicators_listener);
ZMK_SUBSCRIPTION(hid_indicators, zmk_endpoint_changed_indicator);

// 初始化自訂畫面佈局
lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);

    // 左上角：連線狀態
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);

    // 右上角：電池狀態
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);

    // 正中央：大寫狀態提示文字
    caps_label = lv_label_create(screen);
    lv_obj_align(caps_label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(caps_label, "");

    return screen;
}
