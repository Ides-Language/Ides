require 'formula'

class Ides < Formula
	url 'https://github.com/Ides-Language/Ides/archive/master.zip'
	homepage 'http://ides-lang.com'
	version '0.0'
	#md5 '1234567890ABCDEF1234567890ABCDEF'

	depends_on 'cmake'
	depends_on 'boost'
	depends_on 'llvm'
	def install
		#system "./configure", "--prefix=#{prefix}", "--disable-debug", "--disable-dependency-tracking"
		system "cmake", ".", *std_cmake_args
		system "make install"
	end
end
