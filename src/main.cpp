/**
 * BMW S1000RR Style Motorcycle Dashboard
 * for ESP32-S3-LCD-4.3 (Waveshare)
 * 
 * Design: Horizontal TFT layout (800x480)
 * - Top: Fuel bar + Range
 * - Upper: RPM horizontal bar (0-15000, green to red)
 * - Center: Large speed display
 * - Right: Warning lights
 * - Bottom: Water temp, Intake temp, Throttle position
 */

#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

// Display configuration
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define BUF_SIZE (SCREEN_WIDTH * 10)

TFT_eSPI tft = TFT_eSPI();
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[BUF_SIZE];
static lv_color_t buf2[BUF_SIZE];

// UI Elements
static lv_obj_t *rpm_bar;
static lv_obj_t *speed_label;
static lv_obj_t *water_temp_label;
static lv_obj_t *intake_temp_label;
static lv_obj_t *throttle_label;
static lv_obj_t *fuel_bar;
static lv_obj_t *range_label;
static lv_obj_t *time_label;

// Warning indicators
static lv_obj_t *warn_temp_icon;

// Simulated sensor data
float rpm = 0;
float speed = 0;
float waterTemp = 75.0;
float intakeTemp = 25.0;
float throttle = 0;
float fuel = 75.0;
int rpmDirection = 1;

// Colors
#define COLOR_BG        lv_color_hex(0x0a1628)
#define COLOR_TEXT_MAIN lv_color_hex(0xFFFFFF)
#define COLOR_TEXT_DIM  lv_color_hex(0x888888)
#define COLOR_RPM_GREEN lv_color_hex(0x00FF88)
#define COLOR_RPM_RED   lv_color_hex(0xFF0000)
#define COLOR_INFO      lv_color_hex(0x00DDFF)
#define COLOR_WARN      lv_color_hex(0xFF0000)

// Display flushing callback
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();
    
    lv_disp_flush_ready(disp);
}

void create_rpm_scale(lv_obj_t *parent, int y_pos) {
    // Create RPM scale numbers (0-10)
    const char* rpm_labels[] = {"0", "1", "2", "3", "4", "5", "6", "7", "●", "●", "●"};
    int label_count = 11;
    int bar_width = 700;
    int start_x = 50;
    
    for (int i = 0; i < label_count; i++) {
        lv_obj_t *label = lv_label_create(parent);
        lv_label_set_text(label, rpm_labels[i]);
        lv_obj_set_style_text_color(label, i >= 8 ? COLOR_RPM_RED : COLOR_TEXT_DIM, 0);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_18, 0);
        
        int x = start_x + (i * bar_width / (label_count - 1));
        lv_obj_set_pos(label, x - 10, y_pos);
    }
    
    // RPM x1000 label
    lv_obj_t *rpm_unit = lv_label_create(parent);
    lv_label_set_text(rpm_unit, "RPM x1000");
    lv_obj_set_style_text_color(rpm_unit, COLOR_TEXT_DIM, 0);
    lv_obj_set_style_text_font(rpm_unit, &lv_font_montserrat_12, 0);
    lv_obj_set_pos(rpm_unit, start_x, y_pos + 40);
}

void setup_ui() {
    lv_obj_set_style_bg_color(lv_scr_act(), COLOR_BG, 0);
    
    // === TOP: Fuel indicator ===
    lv_obj_t *fuel_icon = lv_label_create(lv_scr_act());
    lv_label_set_text(fuel_icon, LV_SYMBOL_BATTERY_FULL);
    lv_obj_set_style_text_color(fuel_icon, lv_color_hex(0xFFD700), 0);
    lv_obj_set_pos(fuel_icon, 30, 20);
    
    // Fuel bar background
    lv_obj_t *fuel_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(fuel_bg, 120, 8);
    lv_obj_set_pos(fuel_bg, 70, 28);
    lv_obj_set_style_bg_color(fuel_bg, lv_color_hex(0x333333), 0);
    lv_obj_set_style_border_width(fuel_bg, 0, 0);
    lv_obj_set_style_radius(fuel_bg, 4, 0);
    
    // Fuel bar fill
    fuel_bar = lv_obj_create(fuel_bg);
    lv_obj_set_size(fuel_bar, 90, 8); // 75%
    lv_obj_set_pos(fuel_bar, 0, 0);
    lv_obj_set_style_bg_color(fuel_bar, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_border_width(fuel_bar, 0, 0);
    lv_obj_set_style_radius(fuel_bar, 4, 0);
    
    // Range label
    range_label = lv_label_create(lv_scr_act());
    lv_label_set_text(range_label, "--- km");
    lv_obj_set_style_text_color(range_label, lv_color_hex(0xFFD700), 0);
    lv_obj_set_style_text_font(range_label, &lv_font_montserrat_16, 0);
    lv_obj_set_pos(range_label, 200, 22);
    
    // === RPM BAR ===
    create_rpm_scale(lv_scr_act(), 80);
    
    // RPM bar background
    lv_obj_t *rpm_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(rpm_bg, 700, 30);
    lv_obj_set_pos(rpm_bg, 50, 105);
    lv_obj_set_style_bg_color(rpm_bg, lv_color_hex(0x1a1a1a), 0);
    lv_obj_set_style_border_color(rpm_bg, lv_color_hex(0x333333), 0);
    lv_obj_set_style_border_width(rpm_bg, 2, 0);
    lv_obj_set_style_radius(rpm_bg, 15, 0);
    
    // RPM bar fill
    rpm_bar = lv_obj_create(rpm_bg);
    lv_obj_set_size(rpm_bar, 0, 30);
    lv_obj_set_pos(rpm_bar, 0, 0);
    lv_obj_set_style_bg_color(rpm_bar, COLOR_RPM_GREEN, 0);
    lv_obj_set_style_border_width(rpm_bar, 0, 0);
    lv_obj_set_style_radius(rpm_bar, 13, 0);
    
    // === SPEED DISPLAY (CENTER) ===
    speed_label = lv_label_create(lv_scr_act());
    lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_text_color(speed_label, COLOR_TEXT_MAIN, 0);
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_48, 0);
    lv_label_set_text(speed_label, "0");
    
    lv_obj_t *speed_unit = lv_label_create(lv_scr_act());
    lv_obj_align(speed_unit, LV_ALIGN_CENTER, 0, 80);
    lv_obj_set_style_text_color(speed_unit, COLOR_TEXT_DIM, 0);
    lv_obj_set_style_text_font(speed_unit, &lv_font_montserrat_28, 0);
    lv_label_set_text(speed_unit, "km/h");
    
    // === WARNING LIGHTS (RIGHT) ===
    warn_temp_icon = lv_label_create(lv_scr_act());
    lv_label_set_text(warn_temp_icon, LV_SYMBOL_WARNING);
    lv_obj_set_style_text_color(warn_temp_icon, COLOR_WARN, 0);
    lv_obj_set_style_text_font(warn_temp_icon, &lv_font_montserrat_28, 0);
    lv_obj_set_pos(warn_temp_icon, 740, 240);
    lv_obj_add_flag(warn_temp_icon, LV_OBJ_FLAG_HIDDEN); // Hidden by default
    
    // === TIME (BOTTOM RIGHT) ===
    time_label = lv_label_create(lv_scr_act());
    lv_label_set_text(time_label, "03:49 PM");
    lv_obj_set_style_text_color(time_label, COLOR_TEXT_DIM, 0);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_20, 0);
    lv_obj_set_pos(time_label, 680, 440);
    
    // === BOTTOM INFO ===
    // Water temp
    lv_obj_t *h2o_label_text = lv_label_create(lv_scr_act());
    lv_label_set_text(h2o_label_text, "H2O");
    lv_obj_set_style_text_color(h2o_label_text, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(h2o_label_text, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(h2o_label_text, 50, 410);
    
    water_temp_label = lv_label_create(lv_scr_act());
    lv_label_set_text(water_temp_label, "75.0°C");
    lv_obj_set_style_text_color(water_temp_label, COLOR_INFO, 0);
    lv_obj_set_style_text_font(water_temp_label, &lv_font_montserrat_22, 0);
    lv_obj_set_pos(water_temp_label, 50, 430);
    
    // Intake temp
    lv_obj_t *intake_label_text = lv_label_create(lv_scr_act());
    lv_label_set_text(intake_label_text, "Intake");
    lv_obj_set_style_text_color(intake_label_text, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(intake_label_text, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(intake_label_text, 180, 410);
    
    intake_temp_label = lv_label_create(lv_scr_act());
    lv_label_set_text(intake_temp_label, "25.0°C");
    lv_obj_set_style_text_color(intake_temp_label, COLOR_INFO, 0);
    lv_obj_set_style_text_font(intake_temp_label, &lv_font_montserrat_22, 0);
    lv_obj_set_pos(intake_temp_label, 180, 430);
    
    // Throttle
    lv_obj_t *tps_label_text = lv_label_create(lv_scr_act());
    lv_label_set_text(tps_label_text, "TPS");
    lv_obj_set_style_text_color(tps_label_text, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(tps_label_text, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(tps_label_text, 310, 410);
    
    throttle_label = lv_label_create(lv_scr_act());
    lv_label_set_text(throttle_label, "0%");
    lv_obj_set_style_text_color(throttle_label, COLOR_INFO, 0);
    lv_obj_set_style_text_font(throttle_label, &lv_font_montserrat_22, 0);
    lv_obj_set_pos(throttle_label, 310, 430);
}

void update_dashboard() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 100) {
        // Simulate RPM changes
        rpm += rpmDirection * (random(100, 300));
        
        if (rpm >= 14000) {
            rpmDirection = -1;
        } else if (rpm <= 1000) {
            rpmDirection = 1;
        }
        
        rpm = constrain(rpm, 0, 15000);
        
        // Calculate derived values
        speed = rpm / 100.0;
        throttle = map(rpm, 0, 15000, 0, 100);
        waterTemp = 75 + (rpm / 15000.0) * 30;
        intakeTemp = 25 + (throttle / 100.0) * 25;
        
        // Update RPM bar
        int rpmWidth = map(rpm, 0, 15000, 0, 700);
        lv_obj_set_width(rpm_bar, rpmWidth);
        
        // Change color in redzone (12000+ RPM)
        if (rpm >= 12000) {
            lv_obj_set_style_bg_color(rpm_bar, COLOR_RPM_RED, 0);
        } else {
            lv_obj_set_style_bg_color(rpm_bar, COLOR_RPM_GREEN, 0);
        }
        
        // Update speed
        char buf[32];
        sprintf(buf, "%d", (int)speed);
        lv_label_set_text(speed_label, buf);
        
        // Update water temp
        sprintf(buf, "%.1f°C", waterTemp);
        lv_label_set_text(water_temp_label, buf);
        
        // Warning if water temp > 95°C
        if (waterTemp > 95) {
            lv_obj_set_style_text_color(water_temp_label, COLOR_WARN, 0);
            lv_obj_clear_flag(warn_temp_icon, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_set_style_text_color(water_temp_label, COLOR_INFO, 0);
            lv_obj_add_flag(warn_temp_icon, LV_OBJ_FLAG_HIDDEN);
        }
        
        // Update intake temp
        sprintf(buf, "%.1f°C", intakeTemp);
        lv_label_set_text(intake_temp_label, buf);
        
        // Update throttle
        sprintf(buf, "%d%%", (int)throttle);
        lv_label_set_text(throttle_label, buf);
        
        // Update fuel and range
        fuel = max(10.0f, fuel - 0.01f);
        int fuelWidth = map(fuel, 0, 100, 0, 120);
        lv_obj_set_width(fuel_bar, fuelWidth);
        
        int range = (int)((fuel / 100.0) * 350);
        sprintf(buf, "%d km", range);
        lv_label_set_text(range_label, buf);
        
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
    
    Serial.println("S1000RR Dashboard ready!");
}

void loop() {
    lv_timer_handler();
    update_dashboard();
    delay(5);
}
