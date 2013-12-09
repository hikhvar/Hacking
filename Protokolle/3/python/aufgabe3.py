
import hashlib

def read_file(file_name):
    """
        return a list containing all lines of the file.
    """
    words = []
    result_words = []
    try:
        f = open(file_name, "r")
        words = f.readlines()
        for word in words:
            result_words.append(word.strip())
    except Exception, e:
        print "Could not read file", e
    return result_words


def test_all(first_words, second_words, third_words, target_hash):
    """
    test all combination from first_words, second_words and third words.
    Return, when one of the combinations hits the target_hash.
    target_hash should be a hex representation of the hash.
    """
    for word1 in first_words:
        m = hashlib.sha256()
        for word2 in second_words:
            print word1, word2
            for word3 in third_words:
                #print word1, word2, word3, "Hash:"
                #print m2.hexdigest()
                m = hashlib.sha256()
                m.update(word1+word2+word3)
                if m.hexdigest() == target_hash:
                    return word1, word2, word3


def main():
    words = read_file("/home/christoph/GIT/Hacking/Protokolle/3/input/worte-de_all.txt")
    target_hash1 = u"efe1c5a3b960b39ce5a57d4cdda63ba9f75571eb8e2e21cef3aab710c7ca6523"
    target_hash2 = u"e7d7f36656ba468f5c59124f9e94bac3091a8927b839f0634c12d4dec092150c"
    #print test_all([r"Boss"], words, words, target_hash)
    print isinstance(words[0], str) 
    #print test_all(words, ["Kombattant"], words, target_hash2)

if __name__ == "__main__":
    main()
