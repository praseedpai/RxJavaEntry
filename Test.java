

import io.reactivex.Observable;




public class Test {
    
    static String result = "";
    
    public static void main(String args[]){
        result = "";
        Observable<String> observer = Observable.just("Hello"); // provides datea
        observer.subscribe(s -> result=s); // Callable as subscriber
        System.out.println(result);
    }
}