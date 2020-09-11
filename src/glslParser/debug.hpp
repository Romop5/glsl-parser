#ifndef DEBUG_HDR
#define DEBUG_HDR

namespace glsl
{
	class debug
	{
	public:
		static debug& inst() {
			static debug ret;
			return ret;
		}	

		inline void setLine(int line)
		{
			m_line = line;
		}

		inline void advanceLine() { m_line++; }
		
		inline int getLine()
		{
			return m_line;
		}
		
	private:
		int m_line;
	};
}

#endif