require 'formula'

class Ides < Formula
	head 'git://github.com/Ides-Language/Ides.git', :branch => 'master'
	homepage 'http://ides-lang.com'

	depends_on 'cmake'
	depends_on 'boost'
	depends_on 'llvm'
	def install
		system "cmake", ".", *std_cmake_args
		system "make install"
	end
end
