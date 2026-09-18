class Xyzzy < Formula
  desc "Print a brief adventure-style response at the shell"
  homepage "https://github.com/SomerledDesign/xyzzy"
  url "https://github.com/SomerledDesign/xyzzy/releases/download/v0.7.1/xyzzy-0.7.1.tar.gz"
  sha256 "26a2e1e2cd21072e4c2b5c9b8b95b08adda5dcf4be2cea0a2be4382df323f51b"
  license "GPL-2.0-only"

  def install
    system "./configure", "--prefix=#{prefix}"
    system "make"
    system "make", "install"
  end

  test do
    assert_match "xyzzy 0.7.1", shell_output("#{bin}/xyzzy --version")
    assert_path_exists man1/"xyzzy.1"
  end
end
