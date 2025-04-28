using System.Threading.Tasks;
using Firebase.Auth;
using UnityEngine;

namespace FirebaseTools
{
    public class AuthenticationManager : SingletonBehaviour<AuthenticationManager>
    {
        private FirebaseAuth _firebaseAuth;
        public FirebaseAuth firebaseAuth
        {
            get
            {
                if (_firebaseAuth == null)
                    _firebaseAuth = FirebaseAuth.DefaultInstance;
                return _firebaseAuth;
            }
        }

        private FirebaseUser _firebaseUser;

        public FirebaseUser GetFirebaseUser() => _firebaseUser;
        
        public async Task SignInWithEmail(string email, string password)
        {
            var result = await firebaseAuth.SignInWithEmailAndPasswordAsync(email, password);
            if (result == null)
                Debug.LogError($"Sign In With Email Error");
            else
                _firebaseUser = result.User;
        }
        
        public void SignOut() => firebaseAuth.SignOut();
    }
}