#pragma once

#include <Windows.h>
#include "loadffmpeg.h"
#include "sc_Thread.h"
#include "sc_CSLock.h"
#include "sc_Cond.h"
#include "SDL.h"
#include "SDL_thread.h"
#include "StreamHeader.h"

#include <list>
using namespace std;

typedef struct HShowWindow
{
	int  nDrawToolType;
	int  left;
	int  top;
	int  right;
	int  bottom;
	HWND hwnd;
	HShowWindow()
	{
		nDrawToolType = DRAWIAGER_SDL;
		left = 0;
		top = 0;
		right = 0;
		bottom= 0;
		hwnd = NULL;
	}
}HShowWindow;

enum 
{
    AV_SYNC_AUDIO_MASTER, /* default choice */
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

typedef struct AudioParams {
    int freq;
    int channels;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
    int bytes_per_sec;
} AudioParams;

#define VIDEOFRAME 8
#define AUDIOFRAME 9

typedef struct MeidaFrame
{
	int      nType;
	int64_t  pts;
	int64_t  duration;
	unsigned short width;
	unsigned short height;
	unsigned int   nbufsize;
	unsigned char* buf;
	int      nCheck ;
	MeidaFrame()
	{
		pts = 0;
		duration = 0;
		width = 0;
		height = 0;
		nbufsize = 0;
		buf = NULL;
		nCheck = 11199111;
		nType = AUDIOFRAME;
	}
};

class CSourFile
{
public:
	CSourFile(HWND hWnd,int nUseDrawIamgeToolType = DRAWIAGER_DX);
	~CSourFile();
public:
	bool open_file_and_init(const char* szInUrl,const char* szOutUrl);
	bool close_file_and_uninit();
	bool seek_in_file(unsigned int nSeekTime);
	unsigned int get_in_file_duration();
	unsigned int get_in_file_current_play_time();
	bool pause_in_file(bool bIsPause);
	bool switch_paly(bool bflag = false);

public:
	void read_in_file_data_to_proc();
	void decode_video_data();
	void decode_audio_data();
	void send_video_porc();
	void send_audio_porc();

	void send_video_porc(UINT_PTR nEvnet);
	void send_audio_porc(UINT_PTR nEvnet);
	void send_porc(UINT_PTR uTimerID);

	static void  fill_audio(void *udata,Uint8 *stream,int len);
	void OutPouLoop();
private:
	bool copy_video_stream_info(AVStream* ostream, AVStream* istream, AVFormatContext* ofmt_ctx);
	bool copy_audio_stream_info(AVStream* ostream, AVStream* istream, AVFormatContext* ofmt_ctx);
	void improvement_sleep_time(int64_t& n_sleep_time,float& flt_correction_time);
	bool open_in_file_and_get_stream_info();
	bool check_in_file_stream_code_type();
	bool open_out_file_and_add_stream_info();

	bool open_decoder();
	bool close_decoder();

	void get_show_window_width_and_height();

	bool open_sdl_output();
	bool close_sdl_output();
	int audio_open();

private:
	
	void push_busy_pkt(AVPacket* pkt);
	AVPacket* pull_video_busy_pkt();
	AVPacket* pull_audio_busy_pkt();
	AVPacket* get_free_pkt();
	void      put_free_pkt(AVPacket*);

	void push_busy_video_frame(AVFrame * frame);
	MeidaFrame pull_busy_video_frame();

	void push_busy_audio_frame(AVFrame * frame);
	MeidaFrame pull_busy_audio_frame();

	void clear_busy_cache_list(bool bIsCache = true);
	void clear_all_cache_list();

private:

	bool copy_video_frame(AVFrame * frame,MeidaFrame&  mvf);
	bool copy_audio_frame(AVFrame * pframe,MeidaFrame&  maf);

	void  audio_play(void *udata,Uint8 *stream,int len);
	unsigned int get_audio_frame();
private:
	char  m_str_in_file_url[260];
	char  m_str_out_file_url[260];
private:
	bool DrawImageBySDLInit();
	bool DrawImageBySDLUnInit();
	bool DrawImageByDXInit();
	bool DrawImageByDXUnInit();
	bool DrawImageBySDL(unsigned char* video_buf);
	bool DrawImageByDX(unsigned char* pBuf,int nVH,int nVW);

private://ffmpeg
	AVFormatContext * m_in_fmt_ctx; //输入格式上下文
	AVFormatContext	* m_out_fmt_ctx; //输出格式上下文
	AVStream*         m_in_video_st;
	AVStream*		  m_in_audio_st;

	AVStream*		  m_out_video_st;
	AVStream*		  m_out_audio_st;
	AVCodecContext *  m_in_video_codec_ctx;
	AVCodecContext *  m_in_audio_codec_ctx ;

	AVCodec          * m_in_video_codec;
	AVCodec       	 * m_in_audio_codec;

	SwrContext *      m_au_convert_ctx;
	int               m_in_video_index;
	int				  m_in_audio_index;

	bool			  m_is_video_transcoding ; //判断视频是否需要转码
	bool			  m_is_audio_transcoding ; //判断音频是否需要转码

	int				  m_stream_count;  //流得个数
	bool			  m_sync_time_base; //时间轴是否相

	unsigned int      m_pic_size;
	unsigned int      m_aud_size;
private:
	CSCThread         m_read_file_and_proc_thread;

	CSCThread         m_send_video_thread;
	CSCThread         m_send_audio_thread;

	UINT_PTR		  m_send_video_Timer;
	int64_t           m_video_time_count;
	int64_t           m_send_video_prev_pts;
	int64_t           m_send_video_pts;
	int64_t           m_send_busy_video_prev_sys_time;
	int64_t           m_send_free_video_sys_time;
	int64_t           m_video_pkt_pts_time ;

	UINT_PTR		  m_send_audio_Timer;
	int64_t           m_audio_time_count;
	int64_t           m_send_audio_prev_pts;
	int64_t           m_send_audio_pts;
	int64_t           m_send_busy_audio_prev_sys_time;
	int64_t           m_send_free_audio_sys_time;
	int64_t           m_audio_pkt_pts_time ;

	CSCThread         m_decode_video_trhead;  //视频解码线程
	CSCThread         m_decode_audio_trhead;  //音频解码线程
	SDL_Thread*       m_outputThread; //显示线程

	UINT_PTR		  m_send_Timer;
private:
	list<AVPacket*>   m_pool_free_avpacket_list;
	CMutexLock	      m_pool_free_avpacket_lock;

	//send video   
	list<AVPacket*>   m_pool_busy_send_video_avpacket_list;
	CMutexLock	      m_pool_busy_send_video_avpacket_lock;
	//send audio 
	list<AVPacket*>   m_pool_busy_sned_audio_avpacket_list;
	CMutexLock	      m_pool_busy_send_audio_avpacket_lock;

	//video
	list<AVPacket*>   m_pool_busy_video_avpacket_list;
	CMutexLock	      m_pool_busy_video_avpacket_lock;
	CSCCond           m_pool_busy_video_avpacket_cond;

	list<MeidaFrame>   m_pool_busy_video_frame_list;
	CMutexLock	      m_pool_busy_video_frame_lock;
	CSCCond           m_pool_busy_video_frame_cond;
	//audio
	list<AVPacket*>   m_pool_busy_audio_avpacket_list;
	CMutexLock	      m_pool_busy_audio_avpacket_lock;
	CSCCond           m_pool_busy_audio_avpacket_cond;

	list<MeidaFrame>   m_pool_busy_audio_frame_list;
	CMutexLock	      m_pool_busy_audio_frame_lock;

	list<AVPacket*>   m_pool_send_avpacket_list;
	CMutexLock	      m_pool_send_avpacket_lock;

private:
	int      m_video_width;
	int      m_video_height;
	int      m_show_window_width;
	int      m_show_window_height;
	SDL_Surface *   m_pscreen ;  
	SDL_Overlay *   m_poverlay;
	HShowWindow     m_show_rect;

	SDL_AudioSpec   m_wanted_spec;  
	Uint8*			m_audio_buf;
	int				m_audio_buf_len;
	unsigned int    m_audio_buf_size;
	int			    m_audio_pos;
	AudioParams     m_audio_hw_params;
	AudioParams     m_audio_src;
	
private: //video and audio sync
	int64_t         m_video_cur_pts;
	int64_t         m_audio_cur_pts;

	int64_t			m_out_pkt_pts_last_time;
	int64_t			m_out_pkt_dts_last_time;
	int64_t			m_prev_pts;
	int64_t			m_prev_dts;
	bool            m_push_and_play_pause;
	bool            m_bThreshold;
	CMutexLock	    m_play_lock;
	CMutexLock	    m_write_net_lock;
private:
	LPDIRECTDRAW7           m_lpDD;    // DirectDraw 对象指针
	LPDIRECTDRAWSURFACE7    m_lpDDSPrimary;  // DirectDraw 主表面指针
	LPDIRECTDRAWSURFACE7    m_lpDDSOffScr;  // DirectDraw 离屏表面指针
	DDSURFACEDESC2          m_ddsd;    // DirectDraw 表面描述
};