
#include "egl_device_st7789.h"

EGL_USING_NAMESPACE

graphics_device_st7789* graphics_device_st7789::s_active_dev = 0;

graphics_device_st7789::graphics_device_st7789()
{
    EGL_ASSERT(!s_active_dev);
    s_active_dev = this;
    m_tile_rt0   = 0;

    lcd_init(st7789_init_cmd);
    // clear the screen to black
    lcd_select();
    lcd_set_window(0, 0, fb_width - 1, fb_height - 1);
    for (uint32_t i = 0; i < fb_width * fb_height; ++i) {
        lcd_write_data_16bit(0);
			  // lcd_write_data_16bit(0xF800);
    }
    lcd_deselect();
}

graphics_device_st7789::~graphics_device_st7789()
{
    s_active_dev = 0;
}

void graphics_device_st7789::init_rasterizer(const rasterizer_cfg& rcfg_, const rasterizer_tiling_cfg& tcfg_, const rasterizer_vertex_cache_cfg& vccfg_)
{
    graphics_device<graphics_device_st7789>::init(rcfg_, tcfg_, vccfg_);
    m_tile_rt0    = (fb_format_t*)rcfg_.rts[0];
    m_tile_width  = tcfg_.tile_width;
    m_tile_height = tcfg_.tile_height;
}

void graphics_device_st7789::init_dma(rasterizer_data_transfer*, uint8_t num_transfers_, fb_format_t* dma_buffer_, size_t dma_buffer_size_)
{
}

void graphics_device_st7789::submit_tile(uint8_t tx_, uint8_t ty_, const vec2u16& reg_min_, const vec2u16& reg_end_, uint16_t thread_idx_)
{
    // access update pos, size and data
    uint16_t     x             = tx_ * m_tile_width + reg_min_.x;
    uint16_t     y             = ty_ * m_tile_height + reg_min_.y;
    uint16_t     update_width  = reg_end_.x - reg_min_.x;
    uint16_t     update_height = reg_end_.y - reg_min_.y;
    fb_format_t* data          = m_tile_rt0 + reg_min_.x + reg_min_.y * m_tile_width;

    if (m_tile_shader)
        m_tile_shader->transfer_region(render_targets(), depth_target(), size_t(data - m_tile_rt0), x, y, update_width, update_height, m_tile_width);
    else {
        lcd_select();
        lcd_set_window(x, y, x + update_width - 1, y + update_height - 1);
        fb_format_t* data_end = data + m_tile_width * update_height;
        do {
            fb_format_t *data_scan = data, *data_scan_end = data_scan + update_width;
            do {
                lcd_write_data_16bit(data_scan->v);
            } while (++data_scan < data_scan_end);
            data += m_tile_width;
        } while (data < data_end);
        lcd_deselect();
    }
}
