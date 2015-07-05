namespace NETSEC{
	/**
	 * Consumer interface class
	 * 
	 */
	template <typename T, size_t siz>
	class IConsumer{
	protected:
		Buffer<T, siz>& circular_;
		int id_;
		typename Buffer<T, siz>::iterator elem_;
	public:
		Consumer(Buffer<T, siz>& circular);
		virtual ~Consumer();
		void operator()(void);
		virtual void consume() = 0;
	};
}