#ifndef PLAYFILE_H
#define PLAYFILE_H
#include "PlayHeader.h"
#include "MediaDecode.h"
#include "sc_Thread.h"
#include "sc_CSLock.h"
//#include "sc_Cond.h"
#include "SDL.h"
#include "SDL_thread.h"
#include <list>
using namespace std;

enum 
{
    AV_SYNC_AUDIO_MASTER, /* default choice */
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};


typedef struct Clock 
{
    double pts;           /* clock base */
    double pts_drift;     /* clock base minus time at which we updated the clock */
    double last_updated;
    double speed;
    int serial;           /* clock is based on a packet with this serial */
    int paused;
    int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;


typedef struct AudioParams {
    int freq;
    int channels;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
    int bytes_per_sec;
} AudioParams;

typedef void  (*play_audio_callback)(void *udata,Uint8 *stream,int len);

class CPlayFile
{
public:
	CPlayFile();
	~CPlayFile();
public:
	bool  OpenLocalMediaFile(const char* szLocalFile);
	bool  CloseLocalMediaFile();

	void read_in_file_data_to_proc();

	bool decode_one_video_frame(VideoFrame&  vf,int& nSpan);
	bool decode_one_audio_frame(AudioFrame & af);
	int audio_open(play_audio_callback pCallback,void* dwUser);
	void  audio_play(void *udata,unsigned char *stream,int len,int64_t& pts);

	bool seek_in_file(unsigned int nSeekTime);
	bool seek_in_frame(unsigned int nSeekTime,bool video = true);
	bool get_video_frame(unsigned int nFramePos,VideoFrame&  vf,int& nSpan);
	bool get_video_frame(unsigned int nFramePos,VideoFrame&  vf);

	unsigned int get_in_file_duration();
	unsigned int get_in_file_current_play_time();
	unsigned int get_in_file_current_stream_time(bool video = true);

	double CPlayFile::get_video_one_frame_duration();
	bool pause_in_file(bool bIsPause);
	bool switch_paly(play_audio_callback pCallback,void* dwUser,bool bflag = false);

	inline bool  getExistVideoStream() {return m_bIsHaveVideo;}
	inline bool  getExistAudioStream() {return m_bIsHaveAudio;}

	inline bool  getPlayStutas() {return !m_push_and_play_pause;}
private:
	bool open_in_file_and_get_stream_info();
	bool bIsGetMediaPacket();
	unsigned int get_audio_frame(AudioFrame af);
	bool copy_audio_frame(AVFrame * pframe,AudioFrame & af);

	AVPacket* get_free_pkt();
	void      put_free_pkt(AVPacket*);

	void clear_busy_cache_list();
	void clear_all_cache_list();

	bool open_decoder();
	bool close_decoder();

	AVPacket* pull_video_busy_pkt();
	AVPacket* pull_audio_busy_pkt();

private:
	double get_master_clock(int ntype);
	double get_clock(Clock *c);
	int synchronize_audio(int nb_samples);

public:
	char  m_str_in_file_url[260];
	CSCThread m_read_file_and_proc_thread;
	bool            m_push_and_play_pause;
	CMutexLock	    m_play_lock;
	int64_t         m_video_cur_pts;
	int64_t         m_audio_cur_pts;    
	int64_t         m_video_clock ;
	int64_t         m_audio_clock ;
private:
	list<AVPacket*>   m_pool_free_avpacket_list;
	CMutexLock	      m_pool_free_avpacket_lock;

	//video
	list<AVPacket*>   m_pool_busy_video_avpacket_list;
	CMutexLock	      m_pool_busy_video_avpacket_lock;
//	CSCCond           m_pool_busy_video_avpacket_cond;
	//audio
	list<AVPacket*>   m_pool_busy_audio_avpacket_list;
	CMutexLock	      m_pool_busy_audio_avpacket_lock;
//	CSCCond           m_pool_busy_audio_avpacket_cond;

public:
	AVFormatContext * m_in_fmt_ctx; //输入格式上下文

	AVStream*         m_in_video_st;
	AVStream*		  m_in_audio_st;

	AVCodecContext *  m_in_video_codec_ctx;
	AVCodecContext *  m_in_audio_codec_ctx ;

	AVCodec          * m_in_video_codec;
	AVCodec       	 * m_in_audio_codec;

	int               m_in_video_index;
	int				  m_in_audio_index;

	SwrContext *      m_au_convert_ctx;

	unsigned int      m_pic_size;
	unsigned char*    m_pic_bufffer;
	unsigned int      m_aud_size;
	unsigned char*    m_aud_bufffer;

	int64_t           audio_frame_next_pts;
	double            m_video_duration;
	double		      m_audio_stream_timebase;
	double            m_video_stream_timebase;
private:
	AudioParams     m_audio_hw_params;
	AudioParams     m_audio_src;

	SDL_AudioSpec   m_wanted_spec;  
	unsigned char*	m_audio_buf;
	int				m_audio_buf_len;
	unsigned int    m_audio_buf_size;
	int			    m_audio_pos;

private:
	bool            m_bIsHaveVideo;
	bool            m_bIsHaveAudio;

    Clock			m_audclk;
    Clock			m_vidclk;

	int             m_audio_last_serial;
    int				m_audio_clock_serial;
    double			m_audio_diff_cum; /* used for AV difference average computation */
    double			m_audio_diff_avg_coef;
    double			m_audio_diff_threshold ;
	int             m_audio_diff_avg_count;
	int             m_audio_hw_buf_size;

	bool			m_bIsPaly;
private:
	static int      m_openRefCount;
	static CMutexLock   m_refLock;
};
#endif 