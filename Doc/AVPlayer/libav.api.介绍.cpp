﻿/*
@author :
贾增申，qq群中网名：宝玉，qq号：501821782
avPlayer中的libav工程的api的中文介绍，以英文原文为准。
*/

/* 播放器状态. */
typedef enum play_status {
    inited, playing, paused, completed, stoped
} play_status;

enum sync_type {
    AV_SYNC_AUDIO_MASTER, /* 默认选择. */
    AV_SYNC_VIDEO_MASTER, /* 同步到视频时间戳. */
    AV_SYNC_EXTERNAL_CLOCK, /* 同步到外部时钟. */
};

/* 用于config_render参数表示所配置的render或source或demux.  */
#define MEDIA_SOURCE			0
#define MEDIA_DEMUX				1
#define AUDIO_RENDER			2
#define VIDEO_RENDER			3

/* 用于标识渲染器类型. */
#define VIDEO_RENDER_D3D		0
#define VIDEO_RENDER_DDRAW		1
#define VIDEO_RENDER_OPENGL		2
#define VIDEO_RENDER_SOFT		3

/* 队列.	*/
typedef struct _av_queue {
    void *m_first_pkt, *m_last_pkt;
    int m_size; /* 队列大小.	*/
    int m_type; /* 队列类型.	*/
    int abort_request;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
} av_queue;

/* 数据源结构分配和释放. */
EXPORT_API source_context* alloc_media_source(int type, const char *addition, int addition_len, int64_t size);
EXPORT_API void free_media_source(source_context *ctx);

/* 音频结构分配和释放. */
EXPORT_API ao_context* alloc_audio_render();
EXPORT_API void free_audio_render(ao_context *ctx);

/* 视频渲染结构分配和释放. */
EXPORT_API vo_context* alloc_video_render(void *user_data);
EXPORT_API void free_video_render(vo_context *ctx);

/* 分配视频分离和释放. */
EXPORT_API demux_context* alloc_demux_context();
EXPORT_API void free_demux_context(demux_context *ctx);

/* 计算视频实时帧率和实时码率的时间单元. */
#define MAX_CALC_SEC 5

typedef struct avplay {
    /* 文件打开指针. */
    AVFormatContext *m_format_ctx;

    /* 音视频队列.	*/
    av_queue m_audio_q;
    av_queue m_video_q;
    av_queue m_audio_dq;
    av_queue m_video_dq;

    /* 各解码渲染线程.	*/
    pthread_t m_audio_dec_thrd;
    pthread_t m_video_dec_thrd;
    pthread_t m_audio_render_thrd;
    pthread_t m_video_render_thrd;
    pthread_t m_read_pkt_thrd;

    /* 重采样音频指针.	*/
    struct SwsContext *m_swsctx;
    struct SwrContext *m_swr_ctx;
    ReSampleContext *m_resample_ctx;

    /* 音频和视频的AVStream、AVCodecContext指针和index.	*/
    AVCodecContext *m_audio_ctx;
    AVCodecContext *m_video_ctx;
    AVStream *m_audio_st;
    AVStream *m_video_st;
    int m_audio_index;
    int m_video_index;

    /* 读取数据包占用缓冲大小.	*/
    long volatile m_pkt_buffer_size;
    pthread_mutex_t m_buf_size_mtx;

    /* 同步类型. */
    int m_av_sync_type;

    /*
     * 用于计算视频播放时间
     * 即:  m_video_current_pts_drift = m_video_current_pts - time();
     *      m_video_current_pts是当前播放帧的pts时间, 所以在pts向前推进
     *      的同时, time也同样在向前推进, 所以pts_drift基本保存在一个
     *      time_base范围内浮动.
     * 播放时间 = m_video_current_pts_drift - time()
     */
    double m_video_current_pts_drift;
    double m_video_current_pts;

    /* 以下变量用于计算音视频同步.	*/
    double m_frame_timer;
    double m_frame_last_pts;
    double m_frame_last_duration;
    double m_frame_last_delay;
    double m_frame_last_filter_delay;
    double m_frame_last_dropped_pts;
    double m_frame_last_returned_time;
    int64_t m_frame_last_dropped_pos;
    int64_t m_video_current_pos;
    int m_drop_frame_num;

    /* seek实现. */
    int m_read_pause_return;
    int m_seek_req;
    int m_seek_flags;
    int64_t m_seek_pos;
    int64_t m_seek_rel;
    int m_seek_by_bytes;
    int m_seeking;

    /* 最后一个解码帧的pts, 解码帧缓冲大小为2, 也就是当前播放帧的下一帧.	*/
    double m_audio_clock;
    double m_video_clock;
    double m_external_clock;
    double m_external_clock_time;

    /* 当前数据源读取器. */
    source_context *m_source_ctx;
    AVIOContext *m_avio_ctx;
    unsigned char *m_io_buffer;
    /* 用于视频分离的组件. */
    demux_context *m_demux_context;
    /* 当前音频渲染器.	*/
    ao_context *m_ao_ctx;
    /* 当前视频渲染器. */
    vo_context *m_vo_ctx;
    /* 当前音频渲染器是否已经初始化完成, 为1表示完成初始化, 0表示未完成初始化. */
    int m_ao_inited;

    /* 当前音频播放buffer大小.	*/
    uint32_t m_audio_buf_size;

    /* 当前音频已经播放buffer的位置.	*/
    uint32_t m_audio_buf_index;
    int32_t m_audio_write_buf_size;
    double m_audio_current_pts_drift;
    double m_audio_current_pts_last;

    /* 播放状态. */
    play_status m_play_status;
    int m_rendering;

    /* 实时视频输入位率. */
    int m_enable_calc_video_bite;
    int m_real_bit_rate;
    int m_read_bytes[MAX_CALC_SEC]; /* 记录5秒内的字节数. */
    int m_last_vb_time;
    int m_vb_index;

    /* 帧率. */
    int m_enable_calc_frame_rate;
    int m_real_frame_rate;
    int m_frame_num[MAX_CALC_SEC]; /* 记录5秒内的帧数. */
    int m_last_fr_time;
    int m_fr_index;

    /* 正在播放的索引, 只用于BT文件播放. */
    int m_current_play_index;
    double m_start_time;
    double m_buffering;

    /* 停止标志.	*/
    int m_abort;

} avplay;

/*
*作用：	为Player分配上下文（为avplay结构体动态分配内存。）
*输入：	void
*输出：	如果成功返回一个avpaly的结构体指针，
		如果失败返回NULL。
*/
EXPORT_API avplay* alloc_avplay_context();
/*
*作用：	释放由alloc_avplay_context()函数非配的内存，应该与alloc_avplay_context()
		函数成对使用，否则会产生内存泄露。
*输入：	由alloc_avplay_context()函数的返回值的指针。
*输出：	void。
*/
EXPORT_API void free_avplay_context(avplay *ctx);
/*
*作用：	初始化avplay结构。
*输入：	play是有alloc_avplay_context()函数返回的指针。
		sc是“源上下文”，用于读取媒体数据。
*输出：	成功返回0，失败返回AVERROR ,既-1；
*例子：
avplayer *play = alloc_avplay_context();
int ret;
source_context sc = alloc_media_source(MEDIA_TYPE_FILE,"test.mp4",strlen("test.mp4")+1,filesize("test.mp4") );
ret = initialize(play,sc);
if(ret!=0)
	return ret;//出现错误。
*问：在这个函数中都初始化了哪些数据。
*答：1.注册了ffmpeg库中所有的codec，初始化了avformat_network_init()；
	2.为avplay结构中的m_format_ctx初始化。
	3.把avplay中的m_source_ctx=sc。找到正确的数据源。分配内存。
	4.得到audio和video在streams中的index.保存音视频用到的AVStream和AVCodecContext指针。
	5.打开媒体文件对应的音视频解码器。
	6.设置同步方式为默认的AV_SYNC_AUDIO_MASTER（同步音频）方式，
	7。初始化打开媒体文件用到的package。
	8.初始化线程。
	9.把avplay结构中的其余数据均置零。
*/
EXPORT_API int initialize(avplay *play, source_context *sc);
/*
*@author:jeanWin：我认为这个函数是没有实现的函数，这个函数没有做任何东西，
建议不用这个函数，而是用initialize（）函数。
 * Initialize the player.
 * @param play pointer to user-supplied avplayer (allocated by alloc_avplay_context).
 * @param file_name specifies the source file path or url.
 * @param source_type specifies source type, MEDIA_TYPE_FILE or MEDIA_TYPE_BT、
 *  MEDIA_TYPE_HTTP、 MEDIA_TYPE_RTSP、 MEDIA_TYPE_YK.
 * @param dc pointer to user-supplied demux_context object (allocated by alloc_demux_context).
 * @return 0 on success, a negative AVERROR on failure.
 * example:
 * avplayer* play = alloc_avplay_context();
 * int ret;
 * demux_context *dc = alloc_demux_context();
 * ret = initialize_avplay(play, "test.mp4", MEDIA_TYPE_FILE, dc);
 * if (ret != 0)
 *    return ret; // ERROR!

*/
EXPORT_API int initialize_avplay(avplay *play, const char *file_name, int source_type, demux_context *dc);
/*
*作用：	配置play的渲染器或者数据源。
*输入：	play由alloc_avplay_context()函数的返回值。
		param：音视频渲染器或者数据源。
		type：标志类型可以取的值有 ： MEDIA_SOURCE or AUDIO_RENDER、 MEDIA_DEMUX、 VIDEO_RENDER.
*输出	void
*注意	函数没有进行出错处理，由使用者自己来正确的填写类型，例如：如果param为视频渲染器，而type为AUDIO_RENDER，程序将发生运行时错误，而可以编译通过。
*/
EXPORT_API void configure(avplay *play, void *param, int type);

/*
*作用：	play开始运行。
*输入：	play由alloc_avplay_context()函数的返回值。
		fact这里指的是媒体文件播放的开始时间。一般为0.0000
		index正在播放的索引号，仅对bt文件有用。别的文件可以任意写这个参数，
*问：	这个函数内部是如何实现的呢？
*答：	1.开始设置fact和index参数到avplay结构中。
		2.创建了5个线程。作用分别为
			（1）媒体文件解包
			（2）视频解码
			（3）音频解码
			（4）音频渲染
			（5）视频渲染
*/
EXPORT_API int av_start(avplay *play, double fact, int index);
/*
*作用：	空运转等待播放完毕。
*输入：	play由alloc_avplay_context()函数的返回值。
*输出:	void
*问：	这个函数可以由其他的空运转函数代替吗？例如while（1）{}；
*答：	可以，但要保证av_start（）函数所在的线程不被kill掉。
*/
EXPORT_API void wait_for_completion(avplay *play);
/*
*作用：	停止播放
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	void
*问：	他是如何实现停止的作用的呢？
*答：	1.通知各个线程退出，
		2.等待各个线程退出
		3.各个线程退出后，释放资源。
*/
EXPORT_API void av_stop(avplay *play);
/*
*作用：	暂停播放。
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	void
*问：	是如何实现的？
*答：	自己看代码、
*/
EXPORT_API void av_pause(avplay *play);
/*
*作用：	恢复播放。
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	void
*问：	是如何实现的？
*答：	play->m_play_status = playing;
*
*/
EXPORT_API void av_resume(avplay *play);
/*
*作用：	跳转到fact
*输入：	play由alloc_avplay_context()函数的返回值。
		fact跳转到的位置最大值1.0，最小值0.0
*输出：	void
*/
EXPORT_API void av_seek(avplay *play, double fact);
/*
*作用：	设置音量。
*输入：	play由alloc_avplay_context()函数的返回值。
		l 左（left）声道
		r 右（right）声道
*输出：	如果成功返回0，失败返回其他值。
*/
EXPORT_API int av_volume(avplay *play, double l, double r);
/*
*作用：	音频设备是否初始化
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	初始化了返回0，否则其他值。
*/
EXPORT_API int audio_is_inited(avplay *play);
/*
*作用：	静音设置。
*输入：	play由alloc_avplay_context()函数的返回值。
		s如果为真静音，为假，则不静音或恢复到原始的音量。
*输出：	void
*/
EXPORT_API void av_mute_set(avplay *play, int s);
/*
*作用：	返回当前的播放时间。最大值为1.0，最小值为0.0百分比
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	返回当前的播放时间。最大值为1.0，最小值为0.0百分比
*/
EXPORT_API double av_curr_play_time(avplay *play);
/*
*作用：	返回当前的播放时间。单位是秒
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	返回当前的播放时间。单位是秒
*/
EXPORT_API double av_duration(avplay *play);
/*
*作用：	调用av_close（），并把play结构体free掉。调用这个函数之后
		不能再调用free_avplay_context()函数。否则会出现重复free的错误。
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	void
*/
EXPORT_API void av_destory(avplay *play);
/*
*作用：	使能计算帧速率。
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	void
*问：	他是如何实现的？
*答：	play->m_enable_calc_frame_rate = 1;
*/
EXPORT_API void enable_calc_frame_rate(avplay *play);
/*
*作用：	使能计算bit速率
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	void
*/
EXPORT_API void enable_calc_bit_rate(avplay *play);

/*
*作用：	得到bit速率
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	bit速率
 */
EXPORT_API int current_bit_rate(avplay *play);

/*
*作用：	得到帧速率
*输入：	play由alloc_avplay_context()函数的返回值。
*输出：	帧速率
 */
EXPORT_API int current_frame_rate(avplay *play);

/*
*这个函数仅用于bt文件。
 * Get buffer progress.
 * @param play pointer to the player.
 * @This function return buffering(percent).
*/
EXPORT_API double buffering(avplay *play);
/*
*作用：	设置下载的文件的保存路径
*输入：	play由alloc_avplay_context()函数的返回值。
		save_path 保存的路径
*输出：	void
*问：	什么文件可以设置路径
*答：	MEDIA_TYPE_BT，MEDIA_TYPE_YK这两种形式的文件可以。
*/
EXPORT_API void set_download_path(avplay *play, const char *save_path);
/*
*作用：	设置优酷视频的类型
*输入：	play由alloc_avplay_context()函数的返回值。
		type可以取下面的值：
		hd2,mp4,3gp,3gphd,flv,m3u8.
        0   1   2   3     4   5
*输出：	void
*/
EXPORT_API void set_youku_type(avplay *play, int type);
/*
*作用：	把日志写到文件中
*输入：	logfile，日志的文件名。
*输出：成功返回0
*/
EXPORT_API int logger_to_file(const char* logfile);
/*
*作用：关闭log文件。
*输入：	void
*输出：成功返回0，失败返回-1；
*/
EXPORT_API int close_logger_file();
/*
*作用：	设置日志的格式
*输入：	format string that contains the text to be written to log.
*/
EXPORT_API int logger(const char *fmt, ...);

/*
 * Blurring algorithm to the input video.
 * @param frame pointer to the frame.
 * @param fw is the width of the video.
 * @param fh is the height of the video.
 * @param dx is the x start coordinates of the target location.
 * @param dy is the y start coordinates of the target location.
 * @param dcx is width of the target range.
 * @param dcx is height of the target range.
 */
EXPORT_API void blurring(AVFrame *frame,
                         int fw, int fh, int dx, int dy, int dcx, int dcy);

/*
 * Alpha blend image mixing.
 * @param frame pointer to the frame.
 * @param rgba pointer to the RGBA image.
 * @param fw is the width of the video.
 * @param fh is the height of the video.
 * @param rgba_w is the width of the image.
 * @param rgba_h is the height of the image.
 * @param x is the x start coordinates of the target location.
 * @param y is the y start coordinates of the target location.
 */
EXPORT_API void alpha_blend(AVFrame *frame, uint8_t *rgba,
                            int fw, int fh, int rgba_w, int rgba_h, int x, int y);
