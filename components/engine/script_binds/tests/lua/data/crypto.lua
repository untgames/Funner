local function test_hash (fn_name, fn, str)
  print (string.format ("%s('%s')='%s'", fn_name, str, fn (str)))
end

local function test_crypto_encoding (str, encryption, key, init_string)
  print (string.format ("base64(encode('%s', '%s', '%s', '%s')='%s'", str, encryption, key, init_string, Common.Base64.Encode (Common.Crypto.Encode (str, encryption, key, init_string))))
end

function test ()
  test_hash ("md5", Common.Crypto.MD5, "12345678901234567890123456789012345678901234567890123456789012345678901234567890")
  test_hash ("sha256", Common.Crypto.SHA256, "The quick brown fox jumps over the lazy dog")
  test_hash ("sha1", Common.Crypto.SHA1, "service_id=4method=user.getInfov=1.0secret")

  test_crypto_encoding ("Hello world from the AES encrypt", "aes", "ABCDEFGHIJKLMNOP", "")
end
