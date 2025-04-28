using System.Threading.Tasks;
using Firebase.Database;

namespace FirebaseTools
{
    public class RealtimeDatabaseManager : SingletonBehaviour<RealtimeDatabaseManager>
    {
        public DatabaseReference GetDatabaseRoot() => FirebaseDatabase.DefaultInstance.RootReference;

        public async Task SetValue(string path, string str)
        {
            await GetDatabaseRoot().Child(path).SetValueAsync(str);
        }
        
        public async Task<DataSnapshot> GetValue(string path)
        {
            var dataSnapShot = new TaskCompletionSource<DataSnapshot>();
            GetDatabaseRoot().Child(path).GetValueAsync().ContinueWith(task => 
            { 
                if (task.IsCanceled || task.IsFaulted)
                    dataSnapShot.SetException(task.Exception);
                else
                    dataSnapShot.SetResult(task.Result);
            }, TaskScheduler.FromCurrentSynchronizationContext());
    
            return await dataSnapShot.Task;
        }
    }
}
