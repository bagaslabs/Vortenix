#pragma once

namespace Utils
{
    namespace Hash
    {
        static constexpr std::size_t fnv1a(const std::string_view& data)
        {
            // Fowler/Noll/Vo 1a variant.
            std::size_t prime{ 16777619U };
            std::size_t offset_basis{ 2166136261U };

            if constexpr (sizeof(std::size_t) == 8)
            {
                // 64-bit
                prime = 1099511628211ULL;
                offset_basis = 14695981039346656037ULL;
            }

            std::size_t hash{ offset_basis };
            for (auto& c : data)
            {
                hash ^= c;
                hash *= prime;
            }

            return hash;
        }

        static constexpr std::int32_t proton(const char* data, std::size_t length = 0)
        {
            std::int32_t hash{ 0x55555555 };
            if (data)
            {
                if (length > 0)
                {
                    while (length--)
                    {
                        hash = (hash >> 27) + (hash << 5) + *reinterpret_cast<const std::uint8_t*>(data++);
                    }

                    return hash;
                }

                while (*data)
                {
                    hash = (hash >> 27) + (hash << 5) + *reinterpret_cast<const std::uint8_t*>(data++);
                }
            }

            return hash;
        }

        static std::string sha256(const std::string& input)
        {
            std::array<unsigned char, SHA256_DIGEST_LENGTH> digest{};
            SHA256(reinterpret_cast<const unsigned char*>(input.data()), input.length(), digest.data());

            std::ostringstream sha256;
            for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
            {
                sha256 << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
            }

            return sha256.str();
        }

        static std::string md5(const std::string& input)
        {
            unsigned char digest[EVP_MAX_MD_SIZE];
            unsigned int digest_len;

            EVP_MD_CTX* ctx = EVP_MD_CTX_new();
            EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
            EVP_DigestUpdate(ctx, input.c_str(), input.length());
            EVP_DigestFinal_ex(ctx, digest, &digest_len);
            EVP_MD_CTX_free(ctx);

            std::ostringstream md5;
            for (int i = 0; i < 16; ++i)
            {
                md5 << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
            }

            std::string hash = md5.str();
            std::transform(hash.begin(), hash.end(), hash.begin(), ::toupper);
            return hash;
        }
    }
}

constexpr size_t operator"" _fh(const char* left, size_t len)
{
    return Utils::Hash::fnv1a(std::string_view{ left, len });
}
