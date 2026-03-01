/**
 * BMW S1000RR Style Motorcycle Dashboard
 * for ESP32-S3-LCD-4.3 (Waveshare)
 * 
 * Features:
 * - RPM Gauge (0-15000)
 * - Speed Display (km/h)
 * - Water Temperature
 * - Intake Air Temperature  
 * - Throttle Position
 */

#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

// Display setup
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define BUF_SIZE (SCREEN_WIDTH * 10)

TFT_eSPI tft = TFT_eSPI();
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[BUF_SIZE];
static lv_color_t buf2[BUF_SIZE];

// UI Elements
static lv_obj_t *rpm_arc;
static lv_obj_t *rpm_label;
static lv_obj_t *speed_label;
static lv_obj_t *temp_label;
static lv_obj_t *intake_temp_label;
static lv_obj_t *throttle_label;

// Simulated sensor data
float rpm = 0;
float speed = 0;
float waterTemp = 75.0;
float intakeTemp = 25.0;
float throttle = 0;

// Display flushing
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();
    
    lv_disp_flush_ready(disp);
}

void setup_ui() {
    // Set dark theme (S1000RR style)
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0);
    
    // Create RPM Arc Gauge (center)
    rpm_arc = lv_arc_create(lv_scr_act());
    lv_obj_set_size(rpm_arc, 300, 300);
    lv_obj_center(rpm_arc);
    lv_arc_set_range(rpm_arc, 0, 15000);
    lv_arc_set_value(rpm_arc, 0);
    lv_arc_set_bg_angles(rpm_arc, 135, 45);
    
    // RPM Arc colors
    lv_obj_set_style_arc_color(rpm_arc, lv_color_hex(0x2080FF), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(rpm_arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(rpm_arc, lv_color_hex(0x303030), LV_PART_MAIN);
    lv_obj_set_style_arc_width(rpm_arc, 15, LV_PART_MAIN);
    
    // Red line zone (12000-15000 RPM)
    lv_obj_set_style_arc_color(rpm_arc, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_FOCUSED);
    
    // RPM Label
    rpm_label = lv_label_create(lv_scr_act());
    lv_obj_align(rpm_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(rpm_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(rpm_label, &lv_font_montserrat_48, 0);
    lv_label_set_text(rpm_label, "0");
    
    // Speed Display (large, top right)
    speed_label = lv_label_create(lv_scr_act());
    lv_obj_align(speed_label, LV_ALIGN_TOP_RIGHT, -20, 20);
    lv_obj_set_style_text_color(speed_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_48, 0);
    lv_label_set_text(speed_label, "0 km/h");
    
    // Water Temperature (bottom left)
    temp_label = lv_label_create(lv_scr_act());
    lv_obj_align(temp_label, LV_ALIGN_BOTTOM_LEFT, 20, -80);
    lv_obj_set_style_text_color(temp_label, lv_color_hex(0x40B0FF), 0);
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_24, 0);
    lv_label_set_text(temp_label, "H2O: 75°C");
    
    // Intake Air Temperature (bottom left)
    intake_temp_label = lv_label_create(lv_scr_act());
    lv_obj_align(intake_temp_label, LV_ALIGN_BOTTOM_LEFT, 20, -40);
    lv_obj_set_style_text_color(intake_temp_label, lv_color_hex(0x40FF80), 0);
    lv_obj_set_style_text_font(intake_temp_label, &lv_font_montserrat_24, 0);
    lv_label_set_text(intake_temp_label, "Intake: 25°C");
    
    // Throttle Position (bottom right)
    throttle_label = lv_label_create(lv_scr_act());
    lv_obj_align(throttle_label, LV_ALIGN_BOTTOM_RIGHT, -20, -40);
    lv_obj_set_style_text_color(throttle_label, lv_color_hex(0xFFFF00), 0);
    lv_obj_set_style_text_font(throttle_label, &lv_font_montserrat_24, 0);
    lv_label_set_text(throttle_label, "TPS: 0%");
}

void update_dashboard() {
    // Simulate sensor data (replace with real sensor inputs)
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 100) {
        // Simulate RPM changes
        rpm += random(-500, 500);
        rpm = constrain(rpm, 0, 15000);
        
        // Simulate speed
        speed = rpm / 100.0;
        
        // Simulate throttle
        throttle = map(rpm, 0, 15000, 0, 100);
        
        // Update UI
        lv_arc_set_value(rpm_arc, (int)rpm);
        
        char buf[32];
        sprintf(buf, "%d", (int)rpm);
        lv_label_set_text(rpm_label, buf);
        
        sprintf(buf, "%d km/h", (int)speed);
        lv_label_set_text(speed_label, buf);
        
        sprintf(buf, "H2O: %.1f°C", waterTemp);
        lv_label_set_text(temp_label, buf);
        
        sprintf(buf, "Intake: %.1f°C", intakeTemp);
        lv_label_set_text(intake_temp_label, buf);
        
        sprintf(buf, "TPS: %d%%", (int)throttle);
        lv_label_set_text(throttle_label, buf);
        
        // Warning color for high water temp
        if (waterTemp > 95) {
            lv_obj_set_style_text_color(temp_label, lv_color_hex(0xFF0000), 0);
        } else {
            lv_obj_set_style_text_color(temp_label, lv_color_hex(0x40B0FF), 0);
        }
        
        lastUpdate = millis();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("BMW S1000RR Dashboard Starting...");
    
    // Initialize display
    tft.begin();
    tft.setRotation(1); // Landscape
    tft.fillScreen(TFT_BLACK);
    
    // Initialize LVGL
    lv_init();
    
    // Setup display buffer
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, BUF_SIZE);
    
    // Initialize display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    // Create UI
    setup_ui();
    
    Serial.println("Dashboard ready!");
}

void loop() {
    lv_timer_handler(); // LVGL tasks
    update_dashboard(); // Update sensor data
    delay(5);
}
