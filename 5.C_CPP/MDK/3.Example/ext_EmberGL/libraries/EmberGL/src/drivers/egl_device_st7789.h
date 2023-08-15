#ifndef EGL_DEVICE_ST7789_H
#define EGL_DEVICE_ST7789_H
//----------------------------------------------------------------------------

//============================================================================
// interface
//============================================================================
// external
#include "../egl_device.h"
#include "lcd.h"

EGL_NAMESPACE_BEGIN

// new
class graphics_device_st7789;
//----------------------------------------------------------------------------

//============================================================================
// graphics_device_st7789
//============================================================================

class graphics_device_st7789 : public graphics_device<graphics_device_st7789> {
public:
    // device properties
    enum { fb_width  = LCD_WIDTH,
           fb_height = LCD_HEIGHT };
    enum { fb_format = pixfmt_b5g6r5 };
    typedef pixel<e_pixel_format(fb_format)> fb_format_t;

    // construction
    graphics_device_st7789();
    ~graphics_device_st7789();
    void init_rasterizer(const rasterizer_cfg&, const rasterizer_tiling_cfg&, const rasterizer_vertex_cache_cfg&);
    void init_dma(rasterizer_data_transfer*, uint8_t num_transfers_, fb_format_t* dma_buffer_, size_t dma_buffer_size_);

    // immediate rendering interface
    template <class IPShader>
    EGL_INLINE void fast_draw_hline(uint16_t x_, uint16_t y_, uint16_t width_, const IPShader&);
    template <class IPShader>
    EGL_INLINE void fast_draw_vline(uint16_t x_, uint16_t y_, uint16_t height_, const IPShader&);
    template <class IPShader>
    EGL_INLINE void fast_draw_rect(uint16_t x_, uint16_t y_, uint16_t width_, uint16_t height_, const IPShader&);

private:
    virtual void submit_tile(uint8_t tx_, uint8_t ty_, const vec2u16& reg_min_, const vec2u16& reg_end_, uint16_t thread_idx_);

    static graphics_device_st7789* s_active_dev;

    // tile data
    fb_format_t*           m_tile_rt0;
    rasterizer_tile_size_t m_tile_width;
    rasterizer_tile_size_t m_tile_height;
};

//============================================================================
// graphics_device_st7789
//============================================================================
template <class IPShader>
void graphics_device_st7789::fast_draw_hline(uint16_t x_, uint16_t y_, uint16_t width_, const IPShader& ips_)
{
    // draw horizontal line
    uint16_t x_end = x_ + width_;
    lcd_select();
    lcd_set_window(x_, y_, x_end - 1, y_);
    fb_format_t res;
    do {
        ips_.exec(res, x_, y_);
        lcd_write_data_16bit(res.v);
        ++x_;
    } while (x_ < x_end);
    lcd_deselect();
}

template <class IPShader>
void graphics_device_st7789::fast_draw_vline(uint16_t x_, uint16_t y_, uint16_t height_, const IPShader& ips_)
{
    // draw vertical line
    uint16_t y_end = y_ + height_;
    lcd_select();
    lcd_set_window(x_, y_, x_, y_end - 1);
    fb_format_t res;
    do {
        ips_.exec(res, x_, y_);
        lcd_write_data_16bit(res.v);
        ++y_;
    } while (y_ < y_end);
    lcd_deselect();
}

template <class IPShader>
void graphics_device_st7789::fast_draw_rect(uint16_t x_, uint16_t y_, uint16_t width_, uint16_t height_, const IPShader& ips_)
{
    // draw rectangle
    uint16_t x_end = x_ + width_, y_end = y_ + height_;
    lcd_select();
    lcd_set_window(x_, y_, x_end - 1, y_end - 1);
    fb_format_t res;
    do {
        uint16_t x = x_;
        do {
            ips_.exec(res, x, y_);
            lcd_write_data_16bit(res.v);
            ++x;
        } while (x < x_end);
        ++y_;
    } while (y_ < y_end);
    lcd_deselect();
}

EGL_NAMESPACE_END
#endif
